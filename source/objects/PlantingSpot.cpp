//
//  PlantingSpot.cpp
//  Rooted
//
//  Created by Kimmy Lin on 3/11/24.
//

#include "PlantingSpot.h"

bool PlantingSpot::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
        
    if (BoxObstacle::init(pos, size)) {
        // set this so players can pass through
        setSensor(true);
        return true;
    }
    return false;
}

void PlantingSpot::dispose() {
    _node = nullptr;
}

void PlantingSpot::setSceneNode(const std::shared_ptr<cugl::AssetManager> &assets, float priority) {
    _node = scene2::SceneNode::allocWithBounds((Rect(Vec2::ZERO, _drawScale * _dimension)));
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPriority(priority);
    float texscale = assets->get<Texture>("planting")->getWidth()/4.0/_drawScale;
    if (_dimension.width == 1 && _dimension.height == 1){
        addTileNode(Vec2::ZERO, texscale, assets, "planting_single");
    } else if (_dimension.width == 1) {
        addTileNode(Vec2::ZERO, texscale, assets, "planting_v_bot");
        for (int i = 1; i < _dimension.height-1; i++) {
            addTileNode(Vec2(0, i), texscale, assets, "planting_v_mid");
        }
        addTileNode(Vec2(0, _dimension.height-1), texscale, assets, "planting_v_top");
    } else if (_dimension.height == 1) {
        addTileNode(Vec2::ZERO, texscale, assets, "planting_h_left");
        for (int i = 1; i < _dimension.width-1; i++) {
            addTileNode(Vec2(i, 0), texscale, assets, "planting_h_mid");
        }
        addTileNode(Vec2(_dimension.width-1, 0), texscale, assets, "planting_h_right");
    } else {
        
        //top row
        addTileNode(Vec2(0, _dimension.height - 1), texscale, assets, "planting_top_left");
        for (int i = 1; i < _dimension.width-1; i++) {
            addTileNode(Vec2(i, _dimension.height - 1), texscale, assets, "planting_top_mid");
        }
        addTileNode(Vec2(_dimension.width-1, _dimension.height - 1), texscale, assets, "planting_top_right");
        
        //middle row
        for (int j = 1; j < _dimension.height-1; j++) {
            addTileNode(Vec2(0, j), texscale, assets, "planting_mid_left");
            for (int i = 1; i < _dimension.width-1; i++) {
                addTileNode(Vec2(i, j), texscale, assets, "planting_mid_mid");
            }
            addTileNode(Vec2(_dimension.width-1, j), texscale, assets, "planting_mid_right");
        }
        
        //bottom row
        addTileNode(Vec2(0, 0), texscale, assets, "planting_bot_left");
        for (int i = 1; i < _dimension.width - 1; i++) {
            addTileNode(Vec2(i, 0), texscale, assets, "planting_bot_mid");
        }
        addTileNode(Vec2(_dimension.width-1, 0), texscale, assets, "planting_bot_right");
    }
}

void PlantingSpot::addTileNode(Vec2 position, float texscale, const std::shared_ptr<cugl::AssetManager> &assets, std::string texkey) {
    auto child = scene2::PolygonNode::allocWithPoly(Rect(Vec2::ZERO, Size(_drawScale * texscale, _drawScale * texscale)));
    child->setTexture(assets->get<Texture>(texkey));
    child->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    child->setPosition(_drawScale * position);
    child->setScale(1.0/texscale);
    child->setPriority(_node->getPriority());
    _node->addChild(child);
}
