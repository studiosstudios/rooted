//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_ACTIONCONTROLLER_H
#define ROOTED_ACTIONCONTROLLER_H

#include <cugl/cugl.h>
#include "../objects/Map.h"
#include "InputController.h"

class ActionController {
private:
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;

    std::shared_ptr<InputController> _input;

    std::shared_ptr<Map> _map;
public:

    /**
     * Disposes of all resources in this instance of ActionController
     */
    void dispose() {
        _map = nullptr;
        _world = nullptr;
    }

    bool init(std::shared_ptr<Map> &map, std::shared_ptr<InputController> &input);

    void preUpdate(float dt);

    void postUpdate(float remain);
};


#endif //ROOTED_ACTIONCONTROLLER_H
