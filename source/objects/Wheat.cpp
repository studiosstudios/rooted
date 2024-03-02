//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Wheat.h"

#pragma mark -
#pragma mark Physics Constants

/** The amount to shrink the fixture (horizontally) relative to the image */
#define WHEAT_HSHRINK  0.28f
/** The amount to shrink the fixture (vertically) relative to the image */
#define WHEAT_VSHRINK  0.15f

bool Wheat::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    Size nsize = size;
    nsize.width  *= WHEAT_HSHRINK;
    nsize.height *= WHEAT_VSHRINK;
    _drawScale = scale;
        
    if (BoxObstacle::init(pos, nsize)) {
        // set this so players can pass through
        setSensor(true);
                
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
    
    // only change the color of the transparency was not modified with something else in there
    if (_node != nullptr && !_isOccupied) {
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
