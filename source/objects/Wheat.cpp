//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Wheat.h"

bool Wheat::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
    
    if (BoxObstacle::init(pos, size)) {
        // set this so players can pass through
        setSensor(true);
        
        // Gameplay attributes
        _fadeout = 1;
        
        return true;
    }
    return false;
}

void Wheat::dispose() {
    _node = nullptr;
}

void Wheat::update(float dt) {
    // fade out the transparency
    if (_fadeout < 1) {
        _fadeout = (_fadeout+RECOVERY < 1 ? _fadeout+RECOVERY : 1);
    } else {
        _fadeout = 1;
    }
    
    BoxObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setColor(Color4(255, 255, 255, 255 * _fadeout));
    }
}

void Wheat::animateWheat(bool on) {
    if (_fadeout != 1) {
        // Increment and loop
        if (_node->getFrame() != _node->getSpan()-1) {
            _node->setFrame(_node->getFrame()+1);
        } else {
            _node->setFrame(0);
        }
    } else {
        _node->setFrame(0);
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
