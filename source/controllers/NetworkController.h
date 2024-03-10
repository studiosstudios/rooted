//
//  NetworkController.hpp
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//

#ifndef NetworkController_h
#define NetworkController_h

#include <cugl/cugl.h>

using namespace cugl::physics2::net;
using namespace cugl;

class NetworkController {
//    std::shared_ptr<NetEventController> _network;
public:
    std::shared_ptr<NetEventController> _network;
    NetworkController();
    ~NetworkController() { dispose(); }
    void dispose();
    // This shouldn't be like this I think, probably better to just take in assets and make the NetEventController in here
    bool init(std::shared_ptr<NetEventController> network);
    static std::shared_ptr<NetworkController> alloc(std::shared_ptr<NetEventController> network) {
        std::shared_ptr<NetworkController> result = std::make_shared<NetworkController>();
        return (result->init(network) ? result : nullptr);
    }
};

#endif /* NetworkController_h */
