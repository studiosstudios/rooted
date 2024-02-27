//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_BABYCARROT_H
#define ROOTED_BABYCARROT_H

#include "EntityModel.h"

using namespace cugl;

class BabyCarrot : public EntityModel {
    
private:
    
public:
    

    BabyCarrot() {};
    
    ~BabyCarrot() { dispose(); };

    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    static std::shared_ptr<BabyCarrot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<BabyCarrot> result = std::make_shared<BabyCarrot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    void gotCaptured();

};


#endif //ROOTED_BABYCARROT_H
