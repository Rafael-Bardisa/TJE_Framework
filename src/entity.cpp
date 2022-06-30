
#include "entity.h"



int Entity::moveTo(Vector2 dest, float time_elapsed){
    this->position2 = this->position2 + (dest * speed * time_elapsed * 90.f);
    
    this->position3 = this->position2.fromPolar();
    
    return (position2.distance(dest) < speed * time_elapsed * 90.f);
}

int Entity::moveTo(Vector3 dest, float time_elapsed){
    this->position3 = this->position3 + (dest * speed * time_elapsed);
    
    this->position2 = this->position3.toPolar();
    
    return (position3.distance(dest) < speed * time_elapsed);
}
