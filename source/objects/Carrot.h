//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_CARROT_H
#define ROOTED_CARROT_H

#include <cugl/cugl.h>
#include "EntityModel.h"

using namespace cugl;

class Carrot : EntityModel {
    
private:
    bool _isRooted;
    int _numBabyCarrots;
    
public:
    

    Carrot() {};
    
    ~Carrot() {};
    
    bool isRooted() { return _isRooted; };
    
    int getNumBabyCarrots() { return  _numBabyCarrots; };
    
    void captureBabyCarrot();
    
    void gotCaptured();
    
    
};


#endif //ROOTED_CARROT_H
