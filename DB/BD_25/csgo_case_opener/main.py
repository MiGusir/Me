from fastapi import FastAPI, HTTPException, Depends
from fastapi.middleware.cors import CORSMiddleware
from fastapi.openapi.utils import get_openapi
from sqlalchemy.orm import Session
from redis import Redis
from pymongo import MongoClient
from models.postgres_models import User, Case, Item, Inventory, engine, Base
from models.redis_models import get_redis_connection
from models.mongo_models import init_mongo
from typing import List
from pydantic import BaseModel
import random
import os
import json
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)



engine = create_engine("postgresql://postgres:postgres@postgres:5432/csgo")
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

# def init_db():
#     Base.metadata.create_all(bind=engine) 

@app.on_event("startup")
async def startup_event():
    Base.metadata.create_all(bind=engine) 



def get_db():
    db = Session(engine)
    try:
        yield db
    finally:
        db.close()

redis_conn = get_redis_connection()
mongo_client = init_mongo()

class UserCreate(BaseModel):
    username: str
    balance: float = 1000.0

class CaseOpenRequest(BaseModel):
    user_id: int
    case_id: int

class ItemResponse(BaseModel):
    id: int
    name: str
    rarity: str
    price: float

class CaseResponse(BaseModel):
    id: int
    name: str
    price: float
    items: List[ItemResponse]

class InventoryResponse(BaseModel):
    user_id: int
    items: List[ItemResponse]

@app.get("/check_db/")
def check_db(db: Session = Depends(get_db)):
    try:
        users_count = db.query(User).count()
        cases_count = db.query(Case).count()
        items_count = db.query(Item).count()
        
        return {
            "status": "Database initialized",
            "counts": {
                "users": users_count,
                "cases": cases_count,
                "items": items_count
            }
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Database error: {str(e)}")

@app.post("/users/", response_model=UserCreate)
def create_user(user: UserCreate, db: Session = Depends(get_db)):
    existing_user = db.query(User).filter(User.username == user.username).first()
    if existing_user:
        raise HTTPException(
            status_code=400,
            detail="Username already exists"
        )
    
    try:
        db_user = User(username=user.username, balance=user.balance)
        db.add(db_user)
        db.commit()
        db.refresh(db_user)
        
        mongo_db = mongo_client["csgo"]
        mongo_db.users.insert_one({
            "user_id": db_user.id,
            "username": db_user.username,
            "stats": {
                "cases_opened": 0,
                "total_spent": 0,
                "most_valuable_drop": 0
            }
        })
        return UserCreate(username=db_user.username, balance=db_user.balance)
        return db_user
    except Exception as e:
        db.rollback()
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/cases/", response_model=List[CaseResponse])
def get_cases(db: Session = Depends(get_db)):
    cases = db.query(Case).all()
    result = []
    for case in cases:
        items = db.query(Item).filter(Item.case_id == case.id).all()
        item_responses = [ItemResponse(id=item.id, name=item.name, rarity=item.rarity, price=item.price) for item in items]
        result.append(CaseResponse(id=case.id, name=case.name, price=case.price, items=item_responses))
    return result

@app.post("/open_case/", response_model=ItemResponse)
def open_case(request: CaseOpenRequest, db: Session = Depends(get_db)):
    user = db.query(User).filter(User.id == request.user_id).first()
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    
    case = db.query(Case).filter(Case.id == request.case_id).first()
    if not case:
        raise HTTPException(status_code=404, detail="Case not found")
    
    if user.balance < case.price:
        raise HTTPException(status_code=400, detail="Not enough balance")
    
    items = db.query(Item).filter(Item.case_id == case.id).all()
    if not items:
        raise HTTPException(status_code=400, detail="No items in this case")
    
    rarities = {
        "Consumer": 0.8,
        "Industrial": 0.15,
        "Mil-Spec": 0.04,
        "Restricted": 0.008,
        "Classified": 0.0015,
        "Covert": 0.0005
    }
    
    selected_rarity = random.choices(
        list(rarities.keys()),
        weights=list(rarities.values()),
        k=1
    )[0]
    
    rarity_items = [item for item in items if item.rarity == selected_rarity]
    if not rarity_items:
        rarity_items = items 
    
    won_item = random.choice(rarity_items)
    
    user.balance -= case.price
    db.commit()
    
    inventory_item = Inventory(user_id=user.id, item_id=won_item.id)
    db.add(inventory_item)
    db.commit()
    
    mongo_db = mongo_client["csgo"]
    mongo_db.users.update_one(
        {"user_id": user.id},
        {
            "$inc": {
                "stats.cases_opened": 1,
                "stats.total_spent": case.price
            },
            "$max": {
                "stats.most_valuable_drop": won_item.price
            }
        }
    )
    
    redis_key = f"last_case_open:{user.id}"
    redis_conn.setex(
        redis_key,
        300,
        json.dumps({
            "user_id": user.id,
            "case_id": case.id,
            "item_id": won_item.id,
            "item_name": won_item.name,
            "item_rarity": won_item.rarity,
            "item_price": won_item.price
        })
    )
    return ItemResponse(id=won_item.id, name=won_item.name, rarity=won_item.rarity, price=won_item.price)
    return won_item

@app.get("/inventory/{user_id}", response_model=InventoryResponse)
def get_inventory(user_id: int, db: Session = Depends(get_db)):
    inventory_items = db.query(Inventory).filter(Inventory.user_id == user_id).all()
    items = []
    for inv_item in inventory_items:
        item = db.query(Item).filter(Item.id == inv_item.item_id).first()
        if item:
            items.append(ItemResponse(
                id=item.id,
                name=item.name,
                rarity=item.rarity,
                price=item.price
            ))
    
    return InventoryResponse(user_id=user_id, items=items)

@app.get("/user_stats/{user_id}")
def get_user_stats(user_id: int):
    mongo_db = mongo_client["csgo"]
    stats = mongo_db.users.find_one({"user_id": user_id}, {"_id": 0, "stats": 1})
    if not stats:
        raise HTTPException(status_code=404, detail="User stats not found")
    return stats["stats"]

def custom_openapi():
    if app.openapi_schema:
        return app.openapi_schema
    openapi_schema = get_openapi(
        title="CS:GO Case Opener API",
        version="1.0.0",
        description="API for simulating CS:GO case openings",
        routes=app.routes,
    )
    app.openapi_schema = openapi_schema
    return app.openapi_schema

app.openapi = custom_openapi

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)