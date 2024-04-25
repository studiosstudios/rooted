//
//  CollisionController.hpp
//  Rooted
//
//  Created by Jeffrey Zhou on 2/22/24.
//

#ifndef CollisionController_h
#define CollisionController_h

#include <cugl/cugl.h>
#include <cugl/haptics/CUHaptics.h>
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>
#include "../objects/Map.h"
#include "NetworkController.h"
#include "../events/CaptureEvent.h"
#include "../events/UnrootEvent.h"
#include "../events/CaptureBarrotEvent.h"

class CollisionController {
protected:
//  MARK: - Properties
    /** reference to the map */
    std::shared_ptr<Map> _map;
    /** The network controller */
    std::shared_ptr<NetworkController> _network;

public:

//  MARK: - Constructors

    /**
     * Constructs a Collision Controller
     */
    CollisionController() {};

    /**
     * Destructs a Collision Controller
     */
    ~CollisionController() { dispose(); }

    /**
     * Disposes of all resources in this instance of Collision Controller
     */
    void dispose() { _map = nullptr; }

    /**
     * Initializes a Collision Controller
     */
    bool init(std::shared_ptr<Map> &map,  const std::shared_ptr<NetworkController> &network);


//  MARK: - Callbacks

    /**
     * Callback for when two obstacles in the world begin colliding
     */
    void beginContact(b2Contact* contact);

    /**
     * Callback for when two obstacles in the world end colliding
     */
    void endContact(b2Contact* contact);

    /**
     * Callback for determining if two obstacles in the world should collide.
     */
    bool shouldCollide(b2Fixture* f1, b2Fixture* f2);

};

#endif /* CollisionController_h */
