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
    std::shared_ptr<cugl::scene2::SpriteNode> _normalNode;
    std::shared_ptr<cugl::scene2::SpriteNode> _captureNode;
    
    // Animation timers
    float sneakAnimDuration = 5.0f;
    float walkAnimDuration = 2.0f;

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

    void grabCarrot();
    void rootCarrot();
    void carrotEscaped();

    bool canPlant() { return _canPlant; };
    
    void setCanPlant(bool plant) { _canPlant = plant; };
    
    void setNormalNode(std::shared_ptr<cugl::scene2::SpriteNode> n) { _normalNode = n; }
    void setCaptureNode(std::shared_ptr<cugl::scene2::SpriteNode> n) { _captureNode = n; }
    
    void setMovement(cugl::Vec2 movement) override;
    
    void updateCurAnimDurationForState() override;

    void resetFarmer();
};


#endif //ROOTED_FARMER_H
