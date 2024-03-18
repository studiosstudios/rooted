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
    bool _canPlant;
    bool _dashWindow;
    bool _isDashing; //used for farmer-carrot collision when farmer dashes

public:
    int captureTime;
    
public:

    Farmer() {};

    ~Farmer() {};
    
    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);
    
    static std::shared_ptr<Farmer> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Farmer> result = std::make_shared<Farmer>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    bool isDashing(){ return _isDashing; }
    
    bool isHoldingCarrot(){ return _isHoldingCarrot; }

    void grabCarrot();

    void rootCarrot();

    bool canPlant() { return _canPlant; };
    
    void setCanPlant(bool plant) { _canPlant = plant; };
    
    /**
     * Used to detect whether farmer collides with carrot while dashing.
     */
    void setDash(bool dash) {_isDashing = dash; }
};


#endif //ROOTED_FARMER_H
