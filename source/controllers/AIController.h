//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_AICONTROLLER_H
#define ROOTED_AICONTROLLER_H

#include <cugl/cugl.h>
#include "../objects/BabyCarrot.h"
#include "../objects/Map.h"

class AIController {
    
protected:
    std::shared_ptr<Map> _map;
    
public:
    AIController() {};

    ~AIController() {};
    
    bool init(std::shared_ptr<Map> &map);
    
    void updateBabyCarrot(const std::shared_ptr<BabyCarrot> &baby);
    
    void updateBabyCarrotState(const std::shared_ptr<BabyCarrot> &baby);
    
    bool nearTarget(const Vec2 vec1, const Vec2 vec2);

};


#endif //ROOTED_AICONTROLLER_H
