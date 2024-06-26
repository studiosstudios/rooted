//
// Created by Jeffrey Zhou on 3/27/24.
//

#include "EntitiesNode.h"
#include "../RootedConstants.h"

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
    _root = nullptr;
    _wheattex = nullptr;
    _noisetex = nullptr;
    SceneNode::dispose();
}

bool EntitiesNode::init(const std::shared_ptr<scene2::SceneNode> &entitiesNode, const shared_ptr<Texture> &wheattex,
                        const std::shared_ptr<cugl::AssetManager> &assets, bool fullHeight) {
    if (SceneNode::init()) {
        _root = entitiesNode;
        _fullHeight = fullHeight;

        _wheattex = wheattex;
        _noisetex = assets->get<Texture>("shader_noise");
        _coverShader = Shader::alloc(SHADER(coverShaderVert), SHADER(coverShaderFrag));
        _coverShader->setUniform2f("TEXTURE_PIXEL_SIZE", 1.0 / _wheattex->getWidth(), 1.0 / _wheattex->getHeight());
        _coverShader->setUniform2f("SCENE_SIZE", SCENE_WIDTH, SCENE_HEIGHT);
        _coverShader->setUniform1f("wind_speed", WIND_SPEED);
        _coverShader->setUniform2f("wind_direction", WIND_DIRECTION[0], WIND_DIRECTION[1]);
        _coverShader->setUniform1f("MAX_WHEAT_HEIGHT", MAX_WHEAT_HEIGHT * _fullHeight);
        _coverShader->setUniform1f("STEP_SIZE", STEP_SIZE);
        _windTime = 0;
        return true;
    }
    return false;
}

EntitiesNode::~EntitiesNode() {
    dispose();
}

void EntitiesNode::update(float timestep) {
    _windTime += timestep;
    if (_windTime >= 12.53) {
        _windTime = 0;
    }

    _coverShader->bind();
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

    _root->render(batch, transform, tint);

    batch->end();
    _wheattex->unbind();
    _noisetex->unbind();

    //reset to previous state
    batch->setShader(shader);
    batch->begin(perspective);
}
