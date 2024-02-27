//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "BabyCarrot.h"

/**
 * Initializes a new baby carrot at the given position.
 *
 * The baby carrot is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool BabyCarrot::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return EntityModel::init(pos, size, scale);
}

void BabyCarrot::gotCaptured() {
    _isCaptured = true;
//    this->~BabyCarrot();
    this->markRemoved(true);
}
