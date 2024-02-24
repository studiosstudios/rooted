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

    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;

    float _scale;

    std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;


public:

    Map() {};

    ~Map() {};

    void dispose() {
        _carrots.clear();
        _farmers.clear();
        _babies.clear();
        _wheat.clear();
    }

    bool init(const std::shared_ptr<cugl::AssetManager> &assets,
              std::shared_ptr<cugl::physics2::ObstacleWorld> &world,
              const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
              const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
              float scale);

    static std::shared_ptr<Map> alloc(const std::shared_ptr<cugl::AssetManager> assets,
                                      std::shared_ptr<cugl::physics2::ObstacleWorld> world,
                                      const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
                                      const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
                                      float scale) {
        std::shared_ptr<Map> result = std::make_shared<Map>();
        return (result->init(assets, world, worldnode, debugnode, scale) ? result : nullptr);
    }

    std::vector<std::shared_ptr<BabyCarrot>> getBabyCarrots() { return _babies; }

    std::vector<std::shared_ptr<Carrot>> getCarrots() { return _carrots; }

    std::vector<std::shared_ptr<Farmer>> getFarmers() { return _farmers; }

    std::vector<std::shared_ptr<Wheat>> getWheat() { return _wheat; }

    std::shared_ptr<cugl::physics2::ObstacleWorld> getWorld() { return _world; }

    std::shared_ptr<cugl::physics2::BoxObstacle> getGoalDoor() { return _goalDoor; }

    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
                     const std::shared_ptr<cugl::scene2::SceneNode> &node,
                     const std::shared_ptr<cugl::scene2::SceneNode> &worldnode,
                     const std::shared_ptr<cugl::scene2::SceneNode> &debugnode,
                     bool useObjPosition = true);



};


#endif //ROOTED_MAP_H
