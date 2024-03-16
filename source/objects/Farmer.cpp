//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Farmer.h"

bool Farmer::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    _carrotHeld = nullptr;
    _isDashing = false;
    _canPlant = false;
    return EntityModel::init(pos, size, scale);
}

void Farmer::grabCarrot(Carrot* carrot){
    _carrotHeld = carrot;
    carrot->gotCaptured();
}

void Farmer::rootCarrot(){
    CUAssertLog(_carrotHeld != nullptr, "something went wrong");
    _carrotHeld->gotRooted();
    _carrotHeld = nullptr;
}
