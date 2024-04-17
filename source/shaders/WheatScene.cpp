// This is a Scene2Texture that represents the wheat texture given to the shaders in ShaderRenderer
// and EntitiesNode. This allows for a much easier way to adjust wheat heights during gameplay that
// does not involve a lot of for-loops in the shaders themselves.
//
// For a given pixel, the shaders read the value in the red channel as the base height of the wheat,
// the value in the green channel as the height to increase by, and the height in the blue channel
// as the height to decrease by. For example if a given pixel has color (10, 4, 3) then the wheat
// height represented there is 11. Therefore to adjust wheat heights you need to add SceneNodes to
// the root node with corresponding colors as desired. To ensure that colors are blending properly
// you MUST call setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE) on the node. If you want to
// just directly set the wheat height (i.e. not adding or subtracting) then you should draw to the
// red channel with regular blending.
//
// Note that the root node of this scene is in physics coordinates, so any textures added to this
// scene graph need to be scaled down by the draw scale. This is for convenience purposes
// so that other classes are unaware of the size of the wheat texture. However I am not sure if this
// could become an issue with scaling down textures.
//
// Created by Jeffrey Zhou on 4/10/24.
//

#include "WheatScene.h"
#include "../RootedConstants.h"

const std::string fsqShaderFrag =
#include "FSQShader.frag"
;

const std::string fsqShaderVert =
#include "FSQShader.vert"
;

bool WheatScene::init(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale, Vec2 drawScale, Size worldSize) {

    _wheattex = assets->get<Texture>(name);
    _bladeColorScale = bladeColorScale;
    _fsqshader = Shader::alloc(SHADER(fsqShaderVert), SHADER(fsqShaderFrag));
    if (!Scene2Texture::init(0, 0, DEFAULT_WHEAT_TEX_WIDTH * worldSize.width / DEFAULT_WIDTH,
                             DEFAULT_WHEAT_TEX_HEIGHT * worldSize.height / DEFAULT_HEIGHT, false)) {
        return false;
    }

    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setScale(DEFAULT_DRAWSCALE * DEFAULT_WHEAT_TEX_WIDTH / SCENE_WIDTH, DEFAULT_DRAWSCALE * DEFAULT_WHEAT_TEX_HEIGHT / SCENE_HEIGHT);
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(Vec2::ZERO);
    addChild(_rootnode);

    auto wheatnode = scene2::PolygonNode::allocWithTexture(_wheattex);
    wheatnode->setScale(SCENE_WIDTH/_wheattex->getWidth()/drawScale.x, SCENE_HEIGHT/_wheattex->getHeight()/drawScale.y);
    wheatnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    wheatnode->setColor(Color4(255/_bladeColorScale,255/_bladeColorScale,255/_bladeColorScale,255)); //not sure if this will work for all scales
    wheatnode->setPosition(0,0);
    _rootnode->addChild(wheatnode);


    _target->setClearColor(Color4::CLEAR);

    return true;
}

/**
 * Renders the full wheat scene texture to the screen. This should only be used for debugging
 * purposes.
 *
 * @param alpha     transparency of the texture. default value is 1.0
 * @param scale     how much to scale the rgb channels by. default value is 8.5 so max wheat
 *                  height of 30 corresponds to 255
 */
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


