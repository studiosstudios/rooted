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

void EntitiesNode::setWheatTexture(const shared_ptr<Texture>& wheattex) {
    _wheattex = wheattex;
    _coverShader = Shader::alloc(SHADER(coverShaderVert), SHADER(coverShaderFrag));
    _coverShader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0 / _wheattex->getWidth(), 1.0 / _wheattex->getHeight());
    _coverShader->setUniform2f("SCREEN_SIZE", 1024.0, 576.0);
}

void EntitiesNode::update(float zoom, Vec2 cameraPos) {
    //this can definitely somehow be done with the combined mat4 but i dont really know how it
    // works and cant be bothered to figure it out
    _coverShader->bind();
    _coverShader->setUniform1f("camera_zoom", zoom);
    _coverShader->setUniform2f("camera_pos", cameraPos.x, cameraPos.y);
    _coverShader->unbind();
}

void EntitiesNode::draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) {
    Mat4 perspective = Mat4(batch->getPerspective()); //copy perspective
    shared_ptr<Shader> shader = batch->getShader();
    batch->end();

    _wheattex->bind();
    _wheattex->setName("wheat");
    batch->setShader(_coverShader);
    _coverShader->setSampler("grass_tex", _wheattex);
    batch->begin(perspective);

    auto children = _root->getChildren();
    for(auto it = children.begin(); it != children.end(); ++it) {
        (*it)->render(batch, transform, tint);
    }

    batch->end();
    _wheattex->unbind();
    GLuint error = glGetError();
    CUAssertLog(error == GL_NO_ERROR, "Texture: %s", gl_error_name(error).c_str());

    //reset to previous state
    batch->setShader(shader);
    batch->begin(perspective);
}
