//
//  Obstacle.cpp
//  Rooted
//
//  Created by Nicole Thean on 4/29/24.
//

#include "Decoration.h"

bool Decoration::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
    
    _size = size;
        
    if (BoxObstacle::init(pos, _size)) {
        // pass through for now
        setSensor(true);
        return true;
    }
    return false;
}

void Decoration::dispose() {
    _node = nullptr;
}


void Decoration::stepAnimation(float dt) {
    if (_node != nullptr) {
        if (_shouldAnimate) {
            curAnimTime += dt;
            if (curAnimTime > (curAnimDuration)) { curAnimTime = 0;}
            
                // LOOPING style animation
                 _node->setFrame(lround(_node->getSpan() * curAnimTime / curAnimDuration) % _node->getSpan());
        }
        else if (_node->getFrame() != 0) {
            _node->setFrame(0);
            curAnimTime = 0;
        }
    }
}
