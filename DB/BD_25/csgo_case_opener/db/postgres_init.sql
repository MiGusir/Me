-- CREATE TABLE IF NOT EXISTS users (
--     id SERIAL PRIMARY KEY,
--     username VARCHAR(255) UNIQUE NOT NULL,
--     balance FLOAT DEFAULT 1000.0
-- );

DROP TABLE IF EXISTS items;
DROP TABLE IF EXISTS cases;

CREATE TABLE cases (
    id INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    price DECIMAL(10,2) NOT NULL
);

CREATE TABLE items (
    id INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    rarity VARCHAR(50) NOT NULL,
    price DECIMAL(10,2) NOT NULL,
    case_id INT REFERENCES cases(id)
);

-- CREATE TABLE IF NOT EXISTS inventory (
--     id SERIAL PRIMARY KEY,
--     user_id INTEGER REFERENCES users(id),
--     item_id INTEGER REFERENCES items(id)
-- );

-- Create tables and insert sample data
INSERT INTO cases (id, name, price) VALUES 
(1, 'Danger Zone Case', 1.99),
(2, 'Prisma Case', 2.49),
(3, 'Fracture Case', 1.79);

INSERT INTO items (id, name, rarity, price, case_id) VALUES
-- Danger Zone Case items
(1, 'MP5-SD | Phosphor', 'Mil-Spec', 0.50, 1),
(2, 'MAC-10 | Stalker', 'Mil-Spec', 0.45, 1),
(3, 'AWP | Mortis', 'Restricted', 5.99, 1),
(4, 'M4A4 | Magnesium', 'Restricted', 4.50, 1),
(5, 'AK-47 | Asiimov', 'Covert', 120.00, 1),

-- Prisma Case items
(6, 'USP-S | Cortex', 'Mil-Spec', 0.60, 2),
(7, 'Glock-18 | Vogue', 'Mil-Spec', 0.55, 2),
(8, 'AWP | Containment Breach', 'Restricted', 15.99, 2),
(9, 'M4A1-S | Player Two', 'Restricted', 12.50, 2),
(10, 'Butterfly Knife | Fade', 'Covert', 500.00, 2),

-- Fracture Case items
(11, 'P250 | Visions', 'Mil-Spec', 0.40, 3),
(12, 'Five-SeveN | Scrawl', 'Mil-Spec', 0.35, 3),
(13, 'Desert Eagle | Printstream', 'Restricted', 25.99, 3),
(14, 'AK-47 | X-Ray', 'Restricted', 8.50, 3),
(15, 'Stiletto Knife | Crimson Web', 'Covert', 350.00, 3);