#pragma once

typedef unsigned int uint;
typedef unsigned int ADDR;

struct GameObjectEntry {
    uint id;
    ADDR game_object_pointer;
    uint next;
};

struct GameVector {
    float x;
    float y;
    float z;
};

enum GAME_OBJECT_TYPES : unsigned char {
    AREA = 4,
    CREATURE = 5,
    ITEM = 6,
    TRIGGER = 7,
    PROJECTILE = 8,
    PLACEABLE = 9,
    DOOR = 10,
    AREAOFEFFECT = 11,
    WAYPOINT = 12,
    ENCOUNTER = 13,
    STORE = 14,
    SOUND = 16,
};