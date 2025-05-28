from redis import Redis
import os

def get_redis_connection():
    redis_host = os.getenv("REDIS_HOST", "redis")
    return Redis(host=redis_host, port=6379, db=0)