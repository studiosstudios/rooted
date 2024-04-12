//
// Created by Jeffrey Zhou on 4/10/24.
//

#include "WheatScene.h"

const std::string fsqShaderFrag =
#include "FSQShader.frag"
;

const std::string fsqShaderVert =
#include "FSQShader.vert"
;

bool WheatScene::init(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale) {

    _wheattex = assets->get<Texture>(name);
    _bladeColorScale = bladeColorScale;
    _fsqshader = Shader::alloc(SHADER(fsqShaderVert), SHADER(fsqShaderFrag));
    if (!Scene2Texture::init(0,0,_wheattex->getWidth(), _wheattex->getHeight(), false)) {
        return false;
    }

    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setScale(32*_wheattex->getWidth()/1024.0, 32*_wheattex->getHeight()/576.0);
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(Vec2::ZERO);
    addChild(_rootnode);

    auto wheatnode = scene2::PolygonNode::allocWithTexture(_wheattex);
    wheatnode->setScale(1024.0/_wheattex->getWidth()/32, 576.0/_wheattex->getHeight()/32);
    wheatnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    wheatnode->setColor(Color4(255/_bladeColorScale,255/_bladeColorScale,255/_bladeColorScale,255)); //not sure if this will work for all scales
    wheatnode->setPosition(0,0);
    _rootnode->addChild(wheatnode);

    _target->setClearColor(Color4::CLEAR);

    return true;
}

//for debugging purposes
void WheatScene::renderToScreen(float alpha, float scale) {
    GLuint bindpoint = _texture->getBindPoint();
    _texture->setBindPoint(0);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    _fsqshader->bind();
    _fsqshader->setUniform1f("alpha", alpha);
    _fsqshader->setUniform1f("scale", scale);
    _texture->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _texture->unbind();
    _fsqshader->unbind();
    _texture->setBindPoint(bindpoint);
}

void WheatScene::dispose() {
    _wheattex = nullptr;
    _rootnode = nullptr;
    _fsqshader = nullptr;
    Scene2::dispose();
}


