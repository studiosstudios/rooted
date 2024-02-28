//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_ACTIONCONTROLLER_H
#define ROOTED_ACTIONCONTROLLER_H

#include <cugl/cugl.h>
#include "../objects/Map.h"
#include "InputController.h"
#include "AIController.h"

class ActionController {
private:

    /** reference to the box2d world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** reference to the input controller */
    std::shared_ptr<InputController> _input;
    /** reference to the map */
    std::shared_ptr<Map> _map;
    /** ai controller for baby carrots */
    AIController _ai;

public:

    /**
     * Disposes of all resources in this instance of ActionController
     */
    void dispose() {
        _map = nullptr;
        _world = nullptr;
    }

    /**
     * Initializes an ActionController
     */
    bool init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input);

    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * Processes user input and updates objects in the map accordingly.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);

    /**
     * The method called to indicate the end of a deterministic loop.
     *
     * This method is the final portion of the update loop called before any
     * drawing occurs. As such, it should be used to implement any final
     * animation in response to the simulation provided by {@link #fixedUpdate}.
     * In particular, it should be used to interpolate any visual differences
     * between the the simulation timestep and the FPS.
     *
     * @param remain    The amount of time (in seconds) last fixedUpdate
     */
    void postUpdate(float remain);
};


#endif //ROOTED_ACTIONCONTROLLER_H
