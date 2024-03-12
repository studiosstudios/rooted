//
//  PlantingSpot.cpp
//  Rooted
//
//  Created by Kimmy Lin on 3/11/24.
//

#include "PlantingSpot.h"

bool PlantingSpot::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
        
    if (BoxObstacle::init(pos, size)) {
        // set this so players can pass through
        setSensor(true);
        return true;
    }
    return false;
}

void PlantingSpot::dispose() {
    _node = nullptr;
}
