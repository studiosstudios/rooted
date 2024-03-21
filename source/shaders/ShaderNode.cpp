//
// Created by Jeffrey Zhou on 3/21/24.
//

#include "ShaderNode.h"

void ShaderNode::dispose() {
    _renderer = nullptr;
    scene2::SceneNode::dispose();
}

bool ShaderNode::init(const std::shared_ptr<WheatRenderer> &renderer, ShaderNode::ShaderType type) {
    if (SceneNode::init()) {
        _renderer = renderer;
        _type = type;
    }
    return true;
}

void ShaderNode::draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) {
    Mat4 perspective = Mat4(batch->getPerspective()); //copy perspective
    GLenum srcFactorRGB = batch->getSrcBlendRGB();
    GLenum srcFactorAlpha = batch->getSrcBlendAlpha();
    GLenum dstFactorRGB = batch->getDstBlendRGB();
    GLenum dstFactorAlpha = batch->getDstBlendAlpha();
    GLenum blendEquation = batch->getBlendEquation();
    batch->flush();
    batch->end();

    switch (_type) {
        case ShaderType::WHEAT:
            _renderer->renderWheat();
            break;
        case ShaderType::GROUND:
            _renderer->renderGround();
            break;
    }

    //restore spritebatch to previous state
    batch->begin(perspective);
    batch->setSrcBlendFunc(srcFactorAlpha, srcFactorRGB);
    batch->setDstBlendFunc(dstFactorRGB, dstFactorAlpha);
    batch->setBlendEquation(blendEquation);
}
