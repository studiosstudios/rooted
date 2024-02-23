//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_FARMER_H
#define ROOTED_FARMER_H

#include <cugl/cugl.h>
#include "EntityModel.h"

using namespace cugl;

class Farmer : EntityModel {

private:
    bool _isHoldingCarrot;

public:

    Farmer() {};

    ~Farmer() {};

    bool isHoldingCarrot(){ return _isHoldingCarrot; }

    void grabCarrot();

    void rootCarrot();

};


#endif //ROOTED_FARMER_H
