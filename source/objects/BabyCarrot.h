//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_BABYCARROT_H
#define ROOTED_BABYCARROT_H

#include "EntityModel.h"

using namespace cugl;

class BabyCarrot : public EntityModel {
    
private:
    bool _isCaptured;
    Vec2 _target = Vec2::ZERO;
    int _id;
    
public:
    

    BabyCarrot() {};
    
    ~BabyCarrot() { dispose(); };

    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    static std::shared_ptr<BabyCarrot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<BabyCarrot> result = std::make_shared<BabyCarrot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    int getID() { return _id; };

    void setID(int num) { _id = num; };
    
    void gotCaptured();
    
    bool isCaptured() { return _isCaptured; };
    
    bool hasTarget() { return !_target.isZero(); };

    Vec2 getTarget() { return _target; };
    
    void setTarget(const Vec2 newTarget) { _target = newTarget; };
};

#endif //ROOTED_BABYCARROT_H
