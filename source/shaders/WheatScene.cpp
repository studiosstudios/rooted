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

bool WheatScene::init(const shared_ptr<AssetManager> &assets, vector<vector<pair<string, float>>> mapInfo,
                      Vec2 drawScale, Size worldSize) {

    _fsqshader = Shader::alloc(SHADER(fsqShaderVert), SHADER(fsqShaderFrag));
    if (!Scene2Texture::init(0, 0, DEFAULT_WHEAT_TEX_WIDTH * worldSize.width / DEFAULT_WIDTH,
                             DEFAULT_WHEAT_TEX_HEIGHT * worldSize.height / DEFAULT_HEIGHT, false)) {
        return false;
    }

    _worldSize = Size(worldSize);

    _rootnode = scene2::SceneNode::alloc();
    _rootnode->setScale(DEFAULT_DRAWSCALE * DEFAULT_WHEAT_TEX_WIDTH / SCENE_WIDTH, 
                        DEFAULT_DRAWSCALE * DEFAULT_WHEAT_TEX_HEIGHT / SCENE_HEIGHT);
    _rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _rootnode->setPosition(Vec2::ZERO);
    addChild(_rootnode);

    //add wheat texture nodes based on data in mapInfo
    for (int i = 0; i < mapInfo.size(); i++) {
        for (int j = 0; j < mapInfo[i].size(); j++) {
            std::string name = mapInfo[i][j].first;
            if (name != "") {
                auto wheattex = assets->get<Texture>(name);
                float bladeColorScale = mapInfo[i][j].second;
                auto wheatnode = scene2::PolygonNode::allocWithTexture(wheattex);
                _scale = Size(SCENE_WIDTH / wheattex->getWidth() / drawScale.x * MAP_UNIT_WIDTH / worldSize.width,
                              SCENE_HEIGHT / wheattex->getHeight() / drawScale.y * MAP_UNIT_HEIGHT / worldSize.height);
                wheatnode->setScale(_scale);
                wheatnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                wheatnode->setColor(
                                    Color4(255 / bladeColorScale, 255 / bladeColorScale, 255 / bladeColorScale,
                                           255)); //not sure if this will work for all scales
                wheatnode->setPosition(MAP_UNIT_WIDTH * i, MAP_UNIT_HEIGHT * j);
                _rootnode->addChild(wheatnode);
            }
        }
    }
    
    //add wind texture nodes
    for (int i = 0; i < WIND_COUNT; i++) {
        auto windNode = scene2::PolygonNode::allocWithPoly((Rect(Vec2::ZERO, Size(100, SCENE_HEIGHT*mapInfo.size()*2))));
        windNode->setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE);
        windNode->setAngle(15);
        windNode->setScale(_scale);
        windNode->setAnchor(Vec2::ANCHOR_CENTER);
        windNode->setColor(Color4(0, 1, 3, 255));
        windNode->setPosition(((NUMBER_MAP_UNITS+2)*MAP_UNIT_WIDTH + (NUMBER_MAP_UNITS-1)*MAP_UNIT_WIDTH)/(WIND_COUNT) * i, 0 - MAP_UNIT_WIDTH * i);
        _rootnode->addChild(windNode);
        _windNodes.push_back(windNode);
    }

    _target->setClearColor(Color4::CLEAR);

    _queryId = 0;

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
    _rootnode = nullptr;
    _fsqshader = nullptr;
    Scene2::dispose();
}

int WheatScene::addWheatQuery(Vec2 position) {
    _queries.emplace(_queryId, WheatQuery(position, _queryId));
    _queryId++;
    return _queryId-1;
}

bool WheatScene::getWheatQueryResult(unsigned int id) {
    auto res = _queries.find(id);
    if (res == _queries.end()) {
        CUWarn("could not find query %u", id);
    }
    if (!(res->second).resolved) {
        CUWarn("query %u has not been resolved", id);
    }
    return (res->second).result;
}

void WheatScene::doQueries() {
    //bind render target framebuffer
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glBindFramebuffer(GL_FRAMEBUFFER, _target->getFramebuffer());
    glViewport(0, 0, _target->getWidth(), _target->getHeight());

    //read pixel data for each query
    GLubyte pixel[4];
    Vec2 texPos;
    for (auto &kv : _queries) {
        texPos = Vec2(kv.second.pos.x/_worldSize.width, 1.0-kv.second.pos.y/_worldSize.height) * Vec2(_target->getWidth(), _target->getHeight());
        glReadPixels(int(texPos.x), int(texPos.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
        kv.second.resolved = true;
        kv.second.result = pixel[0] > 0;
    }

    //restore default buffer
    Display::get()->restoreRenderTarget();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    _queryId = 0;
}

void WheatScene::clearQueries() {
    _queries.clear();
}

void WheatScene::updateWindEffect() {
    for (auto node : _windNodes) {
        Vec2 new_pos = node->getPosition()+(WIND_SPEED*Vec2(-WIND_DIRECTION[0], WIND_DIRECTION[1]))*_scale;
    
        if (new_pos.x < 0) {
            new_pos = Vec2((NUMBER_MAP_UNITS+1)*MAP_UNIT_WIDTH, 0);
        }
        node->setPosition(new_pos);
    }
}

