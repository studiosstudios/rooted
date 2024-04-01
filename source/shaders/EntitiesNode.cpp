//
// Created by Jeffrey Zhou on 3/27/24.
//

#include "EntitiesNode.h"

const std::string coverShaderFrag =
#include "cover_fragment.frag"
;

const std::string coverShaderVert =
#include "cover_vertex.vert"
;

EntitiesNode::EntitiesNode() :
        SceneNode(),
        _mainRenderTarget(nullptr),
        _coverShader(nullptr),
        _gradient(nullptr),
        _root(nullptr) {
    _classname = "EntitiesNode";
}

void EntitiesNode::dispose() {
    _mainRenderTarget = nullptr;
    _coverShader = nullptr;
    clearNode();
    SceneNode::dispose();
}

bool EntitiesNode::init(int width, int height) {
    if (SceneNode::init()) {
        _mainRenderTarget = RenderTarget::alloc(width, height);
        _mainRenderTarget->setClearColor(Color4::CLEAR);
        _heightRenderTarget = RenderTarget::alloc(width, height, vector<Texture::PixelFormat>{Texture::PixelFormat::RED});
        _heightRenderTarget->setClearColor(Color4::CLEAR);
        _coverShader = Shader::alloc(SHADER(coverShaderVert), SHADER(coverShaderFrag));
        _gradient = Gradient::allocLinear(Color4::WHITE, Color4::BLACK, Vec2::ANCHOR_BOTTOM_CENTER, Vec2::ANCHOR_TOP_CENTER);
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
    _root->removeAllChildren();
    _root = nullptr;
}

void EntitiesNode::addEntityNode(const shared_ptr<cugl::scene2::SceneNode> &entityNode) {
    _root->addChild(entityNode);
}

void EntitiesNode::draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) {
    Mat4 perspective = Mat4(batch->getPerspective()); //copy perspective
    _mainRenderTarget->begin();
    _root->render(batch, Affine2::IDENTITY, Color4::WHITE);
    batch->end();
    _mainRenderTarget->end();

    std::vector<std::shared_ptr<SceneNode>> children = _root->getChildren();
    for(auto it = children.begin(); it != children.end(); ++it) {
        (*it)->setColor(Color4(255,255,255,(*it)->getHeight()));
    }

    batch->begin(perspective);
    _heightRenderTarget->begin();
    batch->setGradient(_gradient);
    _root->render(batch, Affine2::IDENTITY, Color4::WHITE);
    batch->end();
    batch->setGradient(nullptr);
    _heightRenderTarget->end();

    for(auto it = children.begin(); it != children.end(); ++it) {
        (*it)->setColor(Color4::WHITE);
    }

    //copied from TextureRenderer
    _coverShader->bind();
    _mainRenderTarget->getTexture(0)->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _mainRenderTarget->getTexture(0)->unbind();
    _coverShader->unbind();

    batch->begin(perspective);
}
