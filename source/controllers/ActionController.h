//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_ACTIONCONTROLLER_H
#define ROOTED_ACTIONCONTROLLER_H

#include <cugl/cugl.h>
#include "../objects/Map.h"

class ActionController {
private:
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;

    std::shared_ptr<Map> _map;
public:
    void preUpdate(float dt);

    void fixedUpdate(float step);

    void postUpdate(float remain);
};


#endif //ROOTED_ACTIONCONTROLLER_H
