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


