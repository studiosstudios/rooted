//
// Created by Jeffrey Zhou on 2/22/24.
//

#ifndef ROOTED_MAP_H
#define ROOTED_MAP_H

#include "BabyCarrot.h"
#include "Carrot.h"
#include "Farmer.h"
#include "Wheat.h"

class Map {
private:
    std::vector<std::shared_ptr<BabyCarrot>> _babies;

    std::vector<std::shared_ptr<Carrot>> _carrots;
    
    std::vector<std::shared_ptr<Farmer>> _farmers;
    
    std::vector<std::shared_ptr<Wheat>> _wheat;
    
    
public:
    
    Map() {};
    
    ~Map() {};

    bool init();
    
    static std::shared_ptr<Map> alloc();
    
};


#endif //ROOTED_MAP_H
