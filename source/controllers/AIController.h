//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_AICONTROLLER_H
#define ROOTED_AICONTROLLER_H

#include <cugl/cugl.h>
#include "../objects/BabyCarrot.h"
#include "../objects/Carrot.h"
#include "../objects/Farmer.h"
#include "../objects/Map.h"

class AIController {
    
protected:
    std::shared_ptr<Map> _map;

    Rect _babyBounds;
    
public:
    AIController() {};

    ~AIController() {};

    static std::shared_ptr<AIController> alloc(std::shared_ptr<Map> &map) {
        auto res = std::make_shared<AIController>();
        return (res->init(map) ? res : nullptr);
    }

    bool init(std::shared_ptr<Map> &map);
    
    void updateBabyCarrot(const std::shared_ptr<BabyCarrot> &baby);
    
    void updateBabyCarrotState(const std::shared_ptr<BabyCarrot> &baby);
    
    bool nearTarget(const Vec2 vec1, const Vec2 vec2);

    void setBabyBounds(Rect rect) { _babyBounds.set(rect); }
    
    void updateCarrot(const std::shared_ptr<Carrot> &carrot);
    
    void updateFarmer(const std::shared_ptr<Farmer> &farmer);

};


#endif //ROOTED_AICONTROLLER_H
