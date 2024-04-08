//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Farmer.h"

bool Farmer::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return EntityModel::init(pos, size, scale);
}

void Farmer::grabCarrot(){
    _isHoldingCarrot = true;
//    _normalNode->setVisible(false);
//    _captureNode->setVisible(true);
    setSceneNode(_captureNode);
}

void Farmer::rootCarrot(){
    _isHoldingCarrot = false;
//    _captureNode->setVisible(false);
//    _normalNode->setVisible(true);
    setSceneNode(_normalNode);
}
