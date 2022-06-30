
#ifndef entity_h
#define entity_h

#include <stdio.h>
#include "framework.h"

class Entity{
public:
    Vector3 position3;
    Vector2 position2;
    float speed;
    
    int moveTo(Vector3 dest, float time_elapsed);
    int moveTo(Vector2 dest, float time_elapsed);
};

#endif /* sGameData_hpp */
