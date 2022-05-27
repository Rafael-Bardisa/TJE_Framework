//
// Created by Rafael Bardisa Rodes on 24/4/22.
//

#include "stage.h"
#include "game.h"
// renders and updates

Stage::Stage(Game* game) {
    this->reference = game;
};


// renders
void GeoStage::render() {
    return;
}

void BattleStage::render() {
    return;
}

// updates
STAGE_ID GeoStage::update(float seconds_elapsed) {
    return GEOSCAPE;
}

STAGE_ID BattleStage::update(float seconds_elapsed) {
    return BATTLESCAPE;
}
