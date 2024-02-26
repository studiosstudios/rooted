//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Wheat.h"

void Wheat::dispose() {
    _node = nullptr;
}

void Wheat::update(float dt) {
    // fade out the transparency
    if (_fadeout < 1) {
        _fadeout = (_fadeout < 1 ? _fadeout+RECOVERY : 1);
    } else {
        _fadeout = 1;
    }
    
    BoxObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setColor(Color4(255, 255, 255, 255 * _fadeout));
    }
}

void Wheat::rustle(float amount) {
    if (amount == 0) { return; }
    if (amount >= DASH_INTENSITY) {
        _fadeout = DASH_TRANSPARENCY;
    }
    else if (amount >= WALK_INTENSITY) {
        _fadeout = WALK_TRANSPARENCY;
    }
    else {
        _fadeout = SNEAK_TRANSPARENCY;
    }
}
