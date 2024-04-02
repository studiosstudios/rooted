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
        _coverShader(nullptr),
        _root(nullptr) {
    _classname = "EntitiesNode";
}

void EntitiesNode::dispose() {
    _coverShader = nullptr;
    clearNode();
    SceneNode::dispose();
}

bool EntitiesNode::init(const std::shared_ptr<cugl::AssetManager> &assets, string name, float bladeColorScale) {
    if (SceneNode::init()) {
        _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::PRE_ASCEND);
        _root->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
        _root->setPosition(Vec2::ZERO);

        _wheattex = assets->get<Texture>(name);
        _noisetex = assets->get<Texture>("shader_noise");
        _bladeColorScale = bladeColorScale;
        _coverShader = Shader::alloc(SHADER(coverShaderVert), SHADER(coverShaderFrag));
        _coverShader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0 / _wheattex->getWidth(), 1.0 / _wheattex->getHeight());
        _coverShader->setUniform2f("SCREEN_SIZE", 1024.0, 576.0);
        _coverShader->setUniform1f("blade_color_scale", _bladeColorScale);

        _windTime = 0;
        return true;
    }
    return false;
}

EntitiesNode::~EntitiesNode() {
    dispose();
    clearNode();
}

void EntitiesNode::allocNode() {
//    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::PRE_ASCEND);
//    _root->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
//    _root->setPosition(Vec2::ZERO);
}

void EntitiesNode::clearNode() {
    _root->removeAllChildren();
    _root = nullptr;
}

void EntitiesNode::addEntityNode(const shared_ptr<cugl::scene2::SceneNode> &entityNode) {
    _root->addChild(entityNode);
}

void EntitiesNode::update(float timestep, float zoom, Vec2 cameraPos) {
    _windTime += timestep;
    if (_windTime >= 12.53) {
        _windTime = 0;
    }

    //this can definitely somehow be done with the combined mat4 but i dont really know how it
    // works and cant be bothered to figure it out
    _coverShader->bind();
    _coverShader->setUniform1f("camera_zoom", zoom);
    _coverShader->setUniform2f("camera_pos", cameraPos.x, cameraPos.y);
    _coverShader->setUniform1f("WIND_TIME", _windTime);
    _coverShader->unbind();
}

void EntitiesNode::draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) {
    Mat4 perspective = Mat4(batch->getPerspective()); //copy perspective
    shared_ptr<Shader> shader = batch->getShader();
    batch->end();

    _wheattex->bind();
    _noisetex->bind();
    batch->setShader(_coverShader);
    _coverShader->setSampler("grass_tex", _wheattex);
    _coverShader->setSampler("noise_tex", _noisetex);
    batch->begin(perspective);

    auto children = _root->getChildren();
    for(auto it = children.begin(); it != children.end(); ++it) {
        (*it)->render(batch, transform, tint);
    }

    batch->end();
    _wheattex->unbind();
    _noisetex->unbind();

    //reset to previous state
    batch->setShader(shader);
    batch->begin(perspective);
}
