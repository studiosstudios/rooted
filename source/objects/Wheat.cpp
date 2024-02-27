//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Wheat.h"

bool Wheat::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
        
    if (BoxObstacle::init(pos, size)) {
        // set this so players can pass through
        setSensor(true);
        
        BoxObstacle::createFixtures();
        
        // Gameplay attributes
        _fadeout = 1;
        _animframe = 0;
        _numShakes = 0;
        _isShaking = false;
        
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
    
    // should this be in game scene as per design choices?
    animateWheat();
    
    if (_node != nullptr) {
        _node->setColor(Color4(255, 255, 255, 255 * _fadeout));
    }
}

void Wheat::animateWheat() {
    if (_isShaking && _numShakes < 1) {
        // Increment and loop
        if (_animframe >= WHEAT_FRAMES - WHEAT_ANIMATION_SPEED) {
            _animframe = 0;
            _numShakes += 1;
        } else {
            _animframe += WHEAT_ANIMATION_SPEED;
        }
        _node->setFrame((int) _animframe);
    } else {
        _animframe = 0;
        _node->setFrame(0);
        _isShaking = false;
        _numShakes = 0;
    }
}

void Wheat::rustle(float amount) {
    if (amount == 0) { return; }
    _isShaking = true;
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
