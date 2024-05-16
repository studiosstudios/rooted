//
//  Obstacle.cpp
//  Rooted
//
//  Created by Nicole Thean on 4/29/24.
//

#include "Decoration.h"

bool Decoration::init(const cugl::Vec2 &pos, const cugl::Size &size, float scale) {
    _drawScale = scale;
    
    _size = size;
        
    if (BoxObstacle::init(pos, _size)) {
        // pass through for now
        setSensor(true);
        return true;
    }
    return false;
}

void Decoration::dispose() {
    _node = nullptr;
}

void Decoration::setSceneNode(const std::shared_ptr<cugl::AssetManager> &assets, float priority, std::string texkey) {
    _node = scene2::SceneNode::allocWithBounds((Rect(Vec2::ZERO, _drawScale * _dimension)));
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPriority(priority);
    
    

    
//    float texscale = assets->get<Texture>(texkey)->getWidth()/_drawScale;
//    addTileNode(Vec2::ZERO, texscale, assets, texkey);
    
//    Vec2 position = Vec2::ZERO;
//    _node = scene2::SpriteNode
//    _node = scene2::PolygonNode::allocWithPoly(Rect(Vec2::ZERO, Size(_drawScale * texscale, _drawScale * texscale)));
//    _node->setTexture(assets->get<Texture>(texkey));
//    _node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//    _node->setPosition(_drawScale * position);
//    _node->setScale((_size.width > _size.height) ? 1.0 / texscale * _size.width : 1.0 / texscale * _size.height);
//    _node->setPriority(_node->getPriority());
}

void Decoration::addTileNode(Vec2 position, float texscale, const std::shared_ptr<cugl::AssetManager> &assets, std::string texkey) {
    auto child = scene2::PolygonNode::allocWithPoly(Rect(Vec2::ZERO, Size(_drawScale * texscale, _drawScale * texscale)));
    child->setTexture(assets->get<Texture>(texkey));
    child->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    child->setPosition(_drawScale * position);
    child->setScale((_size.width > _size.height) ? 1.0 / texscale * _size.width : 1.0 / texscale * _size.height);
    child->setPriority(_node->getPriority());
    _tilenode = child;
    _node->addChild(child);
}
