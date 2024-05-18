//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_BABYCARROT_H
#define ROOTED_BABYCARROT_H

#include "EntityModel.h"

using namespace cugl;

#define MAX_NUM_FRAMES_STUCK 10

enum class State {
    EVADE,
    ROAM,
    SIT,
    HOLD
};

class BabyCarrot : public EntityModel {
    
private:
    bool _isCaptured;
    Vec2 _target = Vec2::ZERO;
    State _state;
    int _id;
    int _numFramesStuck; //number of frames a barrot has not made significant progress in moving and it's not in SIT state
    float _distanceAwayFromTarget; //distanceAwayFromTarget in previous frame
    bool _needNewTarget; //true if barrot is stuck and needs a new target
    
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
    
    State getState() { return _state; };
    
    void setState(State state) { _state = state; };
    
    /**
     * Checks if this barrot might be stuck
     */
    void handleMaybeStuckBarrot();
    
    /**
     * Indicates to AIController that a new target is needed because this barrot is stuck
     */
    bool getNeedNewTarget() { return _needNewTarget; };
    
    /**
     * Resets the state of stuck barrot
     */
    void resetStuckBarrot();
};

#endif //ROOTED_BABYCARROT_H
