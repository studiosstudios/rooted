//
// Created by Jeffrey Zhou on 4/10/24.
//

#include "WheatScene.h"

bool WheatScene::init(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale) {

    _bladeColorScale = bladeColorScale;
    _wheattex = assets->get<Texture>(name);
    _bladeColorScale = bladeColorScale;
    if (!Scene2Texture::init(0,0,_wheattex->getWidth(), _wheattex->getHeight(), false)) {
        return false;
    }

    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//    _rootnode->setPosition(_offset);

    addChild(_rootnode);

    auto wheatnode = scene2::PolygonNode::allocWithTexture(_wheattex);
    _rootnode->addChild(wheatnode);
    return true;
}

void WheatScene::dispose() {
    _wheattex = nullptr;
    _rootnode = nullptr;
    Scene2::dispose();
}
