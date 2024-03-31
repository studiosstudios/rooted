//
// Created by Jeffrey Zhou on 3/27/24.
//

#include "EntitiesNode.h"

EntitiesNode::EntitiesNode() :
        SceneNode(),
        _renderTarget(nullptr),
        _coverShader(nullptr),
        _root(nullptr) {
    _classname = "EntitiesNode";
}

void EntitiesNode::dispose() {
    _renderTarget = nullptr;
    _coverShader = nullptr;
    clearNode();
    SceneNode::dispose();
}

bool EntitiesNode::init(int width, int height) {
    if (SceneNode::init()) {
        _renderTarget = RenderTarget::alloc(width, height);
        _renderTarget->setClearColor(Color4::CLEAR);
        _renderer = TextureRenderer::alloc();
        //TODO: alloc shader
        return true;
    }
    return false;
}

EntitiesNode::~EntitiesNode() {
    dispose();
    clearNode();
}

void EntitiesNode::allocNode() {
    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::PRE_ASCEND);
    _root->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _root->setPosition(Vec2::ZERO);
}

void EntitiesNode::clearNode() {
    _root->removeFromParent();
    _root->removeAllChildren();
    _root = nullptr;
}

void EntitiesNode::addEntityNode(const shared_ptr<cugl::scene2::SceneNode> &entityNode) {
    _root->addChild(entityNode);
}

void EntitiesNode::draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) {
    _renderTarget->begin();
    _root->render(batch);
    batch->flush();
    _renderTarget->end();

    _renderer->draw(_renderTarget->getTexture(0));
}
