//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Farmer.h"

bool Farmer::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return EntityModel::init(pos, size, scale);
}

void Farmer::dispose() {
    EntityModel::dispose();
    _normalNode = nullptr;
    _captureNode = nullptr;
}

void Farmer::grabCarrot(){
    _isHoldingCarrot = true;
    _state = CARRYING;
    updateSprite(0);
//    _normalNode->setVisible(false);
//    _captureNode->setVisible(true);
//    setSceneNode(_captureNode);
}

void Farmer::rootCarrot(){
    _isHoldingCarrot = false;
    _state = STANDING;
    updateSprite(0);
//    _captureNode->setVisible(false);
//    _normalNode->setVisible(true);
//    setSceneNode(_normalNode);
}

void Farmer::carrotEscaped(){
    _isHoldingCarrot = false;
    setSceneNode(_normalNode);
}

void Farmer::setMovement(Vec2 movement) {
    _movement = movement;
    if (_isHoldingCarrot) {
        return;
    }
    EntityModel::setMovement(movement);
}

EntityModel::DirectionalSprites Farmer::getDirectionalSpritesForState(EntityModel::EntityState state) {
    switch (state) {
        case CARRYING:
            return _baseCarrySprites;
            break;
        default:
            return EntityModel::getDirectionalSpritesForState(state);
    }
}


void Farmer::updateCurAnimDurationForState() {
    switch (_state) {
            // TODO: This needs to be split up into the correct animDurations for each type of movement states
        case SNEAKING:
            curAnimDuration = sneakAnimDuration;
            break;
        case WALKING:
            curAnimDuration = walkAnimDuration;
            break;
        case RUNNING: {
            curAnimDuration = runAnimDuration;
            break;
        }
        default: {
            // This state duration is not implemented yet
            return;
        }
    }
}

void Farmer::resetFarmer(){
    _isHoldingCarrot = false;
    _canPlant = false;

}
