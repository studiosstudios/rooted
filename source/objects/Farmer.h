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
    
    // Animation timers
    float sneakAnimDuration = 2.0f;
    float walkAnimDuration = 1.0f;
    float runAnimDuration = 0.5f;
    
    std::map<CarrotType, DirectionalSprites> _carryingSprites;
    CarrotType _carriedCarrotType;

public:
    int captureTime;
    
public:

    Farmer() {};

    ~Farmer() override { dispose(); };
    
    void dispose();
    
    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale) override;
    
    static std::shared_ptr<Farmer> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Farmer> result = std::make_shared<Farmer>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    
    bool isHoldingCarrot(){ return _isHoldingCarrot; }

    void grabCarrot(CarrotType carrotType);
    void rootCarrot();
    void carrotEscaped();

    bool canPlant() { return _canPlant; };
    
    void setCanPlant(bool plant) { _canPlant = plant; };    
    
    void setMovement(cugl::Vec2 movement) override;
    
    void appendCarrySprite(CarrotType ct, DirectionalSprites ds) {
        _carryingSprites.insert({ct, ds});
    }
    
    DirectionalSprites getDirectionalSpritesForState(EntityState state) override;
    

    void updateCurAnimDurationForState() override;

    void resetFarmer();
};


#endif //ROOTED_FARMER_H
