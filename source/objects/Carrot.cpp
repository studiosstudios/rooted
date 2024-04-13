//
// Created by Jeffrey Zhou on 2/22/24.
//

#include "Carrot.h"

/**
 * Initializes a new Carrot at the given position.
 *
 * The Carrot is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the Carrot in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Carrot::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    _isCaptured = false;
    _isRooted = false;
    _numBabyCarrots = 0;
    return EntityModel::init(pos, size, scale);
}

/**
 * Sets carrot state after being captured
 */
void Carrot::gotCaptured(){
    _isCaptured = true;
    getSceneNode()->setVisible(false);
}

/**
 * Sets carrot state after getting rooted
 */
void Carrot::gotRooted(){
    _isCaptured = false;
    _isRooted = true;
    getSceneNode()->setVisible(true);
}

/**
 * Sets carrot state after getting unrooted
 */
void Carrot::gotUnrooted(){
    _isRooted = false;
}


void Carrot::updateCurAnimDurationForState() {
    switch (_state) {
        // TODO: This needs to be split up into the correct animDurations for each type of movement states
        case SNEAKING:
        case WALKING:
        case RUNNING: {
            curAnimDuration = walkAnimDuration;
            break;
        }
        default: {
            // This state duration is not implemented yet
            return;
        }
    }
}
