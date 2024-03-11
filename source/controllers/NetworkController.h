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

class NetworkController : public NetEventController {
public:
    NetworkController() {} ; 
    ~NetworkController() { dispose(); }
    void dispose();
    // This shouldn't be like this I think, probably better to just take in assets and make the NetEventController in here
    bool init(const std::shared_ptr<AssetManager>& assets);
    
    static std::shared_ptr<NetworkController> alloc(const std::shared_ptr<AssetManager>& assets) {
        std::shared_ptr<NetworkController> result = std::make_shared<NetworkController>();
        return (result->init(assets) ? result : nullptr);
    }

    std::shared_ptr<cugl::net::NetcodeConnection> getNetcode();
};

#endif /* NetworkController_h */
