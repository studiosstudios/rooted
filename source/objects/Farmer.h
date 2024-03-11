//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_FARMER_H
#define ROOTED_FARMER_H

#include <cugl/cugl.h>
#include "EntityModel.h"

using namespace cugl;

class Farmer : public EntityModel {

private:
    bool _isHoldingCarrot;
    bool _isDashing;

public:

    Farmer() {};

    ~Farmer() {};
    
    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);
    
    static std::shared_ptr<Farmer> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Farmer> result = std::make_shared<Farmer>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    void setDash(bool dash) {_isDashing = dash; }
    
    bool isDashing(){ return _isDashing; }
    
    bool isHoldingCarrot(){ return _isHoldingCarrot; }

    void grabCarrot();

    void rootCarrot();

};


#endif //ROOTED_FARMER_H
