from pymongo import MongoClient
import os

def init_mongo():
    mongo_host = os.getenv("MONGO_HOST", "mongo")
    mongo_user = os.getenv("MONGO_INITDB_ROOT_USERNAME", "root")
    mongo_pass = os.getenv("MONGO_INITDB_ROOT_PASSWORD", "example")
    
    connection_string = f"mongodb://{mongo_user}:{mongo_pass}@{mongo_host}:27017/"
    return MongoClient(connection_string)