//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_CARROT_H
#define ROOTED_CARROT_H

#include "EntityModel.h"

using namespace cugl;

class Carrot : public EntityModel {
    
private:
    bool _isRooted;
    int _numBabyCarrots;
    
public:
    

    Carrot() {};
    
    ~Carrot() { dispose(); };

    bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    static std::shared_ptr<Carrot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Carrot> result = std::make_shared<Carrot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }
    
    bool isRooted() { return _isRooted; };
    
    int getNumBabyCarrots() { return  _numBabyCarrots; };
    
    void captureBabyCarrot() { _numBabyCarrots++; };
    
    void gotCaptured();
    
    
};


#endif //ROOTED_CARROT_H
