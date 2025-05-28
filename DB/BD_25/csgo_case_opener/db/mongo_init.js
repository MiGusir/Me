// Initialize MongoDB with collections
db = db.getSiblingDB('csgo');

// Create users collection
db.createCollection("users");