//
// Created by Rafael Bardisa Rodes on 24/4/22.
//

#ifndef COMPUTERGRAPHICS_SCENE_H
#define COMPUTERGRAPHICS_SCENE_H

#include <cstdlib>
#include "input.h"

class Game;

enum STAGE_ID {
    GEOSCAPE, BATTLESCAPE, NONE
};

class Stage{

public:

    //frames since being active
    long localFrame;
    float epoch;
    float time;
    float elapsed_time;

    Game* reference;

    virtual STAGE_ID GetId() {return NONE;};
    virtual void render() {};
    virtual STAGE_ID update(float seconds_elapsed) {return NONE;};

    Stage(Game* game);
};

class GeoStage : public Stage{
public:
    virtual STAGE_ID GetId(){return GEOSCAPE;};
    virtual void render();
    virtual STAGE_ID update(float seconds_elapsed);
    
    GeoStage(Game* game) : Stage(game){
        
    };
};

class BattleStage : public Stage{
public:
    virtual STAGE_ID GetId(){return BATTLESCAPE;};
    virtual void render();
    virtual STAGE_ID update(float seconds_elapsed);
    
    BattleStage(Game* game) : Stage(game){
        
    };
};

#endif
