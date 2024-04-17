//
//  WheatRenderer.cpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#include "ShaderRenderer.h"

using namespace cugl;

#pragma mark -
#pragma mark Asset Constants
/** The base texture for the wheat shader */
# define BASE_TEXTURE       "shader_base"
/** The cloud texture for the cloud shader */
#define CLOUD_TEXTURE       "shader_clouds"
/** The noise texture for the shader */
#define NOISE_TEXTURE       "shader_noise"
/** The gradient texture for the wheat shader */
#define GRADIENT_TEXTURE    "shader_gradient"
/** The texture for the side of the wheat for the wheat shader */
#define WHEAT_SIDE_TEXTURE  "shader_wheat_side"
/** The texture for the top of the wheat for the wheat shader */
#define WHEAT_TOP_TEXTURE   "shader_wheat_top"

#define WIND_SPEED 1.0
#define CLOUD_SPEED 0.05

/**
 * Wheat fragment shader
 */
const std::string wheatFrag =
#include "wheat_fragment.frag"
;

/**
 * Default vertex shader
 */
const std::string wheatVert =
#include "wheat_vertex.vert"
;

/**
 * Ground fragment shader
 */
const std::string groundFrag =
#include "ground_fragment.frag"
;

/**
 * Default vertex shader
 */
const std::string groundVert =
#include "ground_vertex.vert"
;

/**
 * Cloud fragment shader
 */
const std::string cloudsFrag =
#include "clouds_fragment.frag"
;

/**
 * Default vertex shader
 */
const std::string cloudsVert =
#include "clouds_vertex.vert"
;

using namespace std;

bool ShaderRenderer::init(const shared_ptr<Texture> &wheattex, const std::shared_ptr<cugl::AssetManager> &assets, string name, float bladeColorScale, Size size, bool fullHeight) {
    
    _assets = assets;
    _fullHeight = fullHeight;

    _windTime = 0;
    _cloudTime = 0;

    _bladeColorScale = bladeColorScale;

    _cloudtex = _assets->get<Texture>(CLOUD_TEXTURE);
    _noisetex = _assets->get<Texture>(NOISE_TEXTURE);
    _gradienttex = _assets->get<Texture>(GRADIENT_TEXTURE);
    _grassgradienttex = _assets->get<Texture>("shader_grass_gradient");
    _wheatsidetex = _assets->get<Texture>(WHEAT_SIDE_TEXTURE);
    _wheattoptex = _assets->get<Texture>(WHEAT_TOP_TEXTURE);
    _wheattex = wheattex;

    _size = _wheattex->getSize();

    if (size.height < size.width) {
        _size *= size.width/_size.width;
    } else {
        _size *= size.height/_size.height;
    }
    
    _aspectRatio = _size.width/_size.height;

    _textures.push_back(_cloudtex);
    _textures.push_back(_noisetex);
    _textures.push_back(_gradienttex);
    _textures.push_back(_grassgradienttex);
    _textures.push_back(_wheatsidetex);
    _textures.push_back(_wheattoptex);
    _textures.push_back(_wheattex);

    for (int i = 0; i < _textures.size(); i++) {
        _textures[i]->setBindPoint(i);
    }
    
    return true;
    
}

void ShaderRenderer::dispose() {
    _wheatShader = nullptr;
    _groundShader = nullptr;
    _vertbuff = nullptr;
    _assets = nullptr;
    _textures.clear();
}

void ShaderRenderer::update(float timestep, const Mat4& perspective, int size, float *positions, float *velocities, Vec2 playerPos) {

    _windTime += timestep;
    if (_windTime >= 12.53) {
        _windTime = 0;
    }
    
    _cloudTime += timestep/4;
    if (_cloudTime >= 28.3) {
        _cloudTime = 0;
    }

    if (_wheatShader) {
        _wheatShader->bind();
        _wheatShader->setUniform1f("WIND_TIME", _windTime);
        _wheatShader->setUniform1i("num_entities", size);
        _wheatShader->setUniformMat4("uPerspective", perspective);
        _wheatShader->setUniform2fv("positions", size, positions);
        _wheatShader->setUniform1fv("velocities", size, velocities);
        _wheatShader->setUniform2f("player_pos", playerPos.x, 1 - playerPos.y);
        _wheatShader->unbind();
    }

    if (_groundShader) {
        _groundShader->bind();
        _groundShader->setUniform1f("WIND_TIME", _windTime);
        _groundShader->setUniform1i("num_entities", size);
        _groundShader->setUniformMat4("uPerspective", perspective);
        _groundShader->setUniform2fv("positions", size, positions);
        _groundShader->unbind();
    }

    if (_cloudsShader) {
        _cloudsShader->bind();
        _cloudsShader->setUniform1f("WIND_TIME", _windTime);
        _cloudsShader->setUniform1f("CLOUD_TIME", _cloudTime);
        _cloudsShader->setUniformMat4("uPerspective", perspective);
        _cloudsShader->unbind();
    }

}

void ShaderRenderer::renderWheat() {
    // OpenGL commands to enable alpha blending (if needed)
//    _shader->bind();
    
    if (_wheatShader) {
        _vertbuff->attach(_wheatShader);
        
        // Draw with the given textures (if it exists)
        for (auto texture : _textures) {
            if (texture) {
                texture->bind();
            }
        }

        _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 1);

        _vertbuff->detach();

        for (auto texture : _textures) {
            if (texture) {
                texture->unbind();
            }
        }
    }
}

void ShaderRenderer::renderGround() {
    // OpenGL commands to enable alpha blending (if needed)
//    _shader->bind();

    if (_wheatShader) {
        _vertbuff->attach(_groundShader);

        for (auto texture : _textures) {
            if (texture) {
                texture->bind();
            }
        }

        _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 1);

        for (auto texture : _textures) {
            if (texture) {
                texture->unbind();
            }
        }
        _vertbuff->detach();
    }
}

void ShaderRenderer::renderClouds() {
    // OpenGL commands to enable alpha blending (if needed)
//    _shader->bind();

    if (_cloudsShader) {
        _vertbuff->attach(_cloudsShader);

        for (auto texture : _textures) {
            if (texture) {
                texture->bind();
            }
        }

        _vertbuff->draw(_mesh.command, (int)_mesh.indices.size(), 1);

        for (auto texture : _textures) {
            if (texture) {
                texture->unbind();
            }
        }
        _vertbuff->detach();
    }
}

void ShaderRenderer::buildShaders() {

    CULog("scale: %f \t size: (%f, %f)", _scale, _size.width, _size.height);
    
    
    // Allocate the shader (this binds as well)
    _wheatShader = Shader::alloc(SHADER(wheatVert), SHADER(wheatFrag));
    _wheatShader->setSampler("grass_tex", _wheattex);
    _wheatShader->setSampler("noise_tex", _noisetex);
    _wheatShader->setSampler("gradient_tex", _gradienttex);
    _wheatShader->setSampler("wheat_side_tex", _wheatsidetex);
    _wheatShader->setSampler("wheat_top_tex", _wheattoptex);
    _wheatShader->setUniform1f("WIND_TIME", _windTime);
    _wheatShader->setUniform4f("tip_color", 1.0, 0.866667, 0.231373, 1.0);
    _wheatShader->setUniform4f("wind_color", 1.0, 0.9058824, 0.309804, 1.0);
    _wheatShader->setUniform1f("wind_speed", WIND_SPEED);
    _wheatShader->setUniform2f("wind_direction", 1.0, 1.0);
    _wheatShader->setUniform1f("player_transparency", 0.6);
    _wheatShader->setUniform1f("transparency_radius",20.0);
    _wheatShader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0 / _wheattex->getWidth(), 1.0 / _wheattex->getHeight());
    _wheatShader->setUniform1f("blade_color_scale", _bladeColorScale);
    _wheatShader->setUniform1f("MAX_BLADE_LENGTH", 30 * _fullHeight);

    _groundShader = Shader::alloc(SHADER(groundVert), SHADER(groundFrag));
    _groundShader->setSampler("noise_tex", _noisetex);
    _groundShader->setSampler("grass_tex", _wheattex);
    _groundShader->setSampler("gradient_tex", _grassgradienttex);
    _groundShader->setUniform1f("WIND_TIME", _windTime);
    _groundShader->setUniform1f("wind_speed", WIND_SPEED);
    _groundShader->setUniform2f("wind_direction", 1.0, 1.0);
    _groundShader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0/_wheattex->getWidth(),1.0/_wheattex->getHeight());

    _cloudsShader = Shader::alloc(SHADER(cloudsVert), SHADER(cloudsFrag));
    _cloudsShader->setSampler("clouds_tex", _cloudtex);
    _cloudsShader->setSampler("noise_tex", _noisetex);
    _cloudsShader->setUniform1f("WIND_TIME", _windTime);
    _cloudsShader->setUniform1f("CLOUD_TIME", _cloudTime);
    _cloudsShader->setUniform1f("cloud_speed", CLOUD_SPEED);
    _cloudsShader->setUniform2f("wind_direction", 1.0, 1.0);
    _cloudsShader->setUniform1f("cloud_alpha_scale", 0.5);
    _cloudsShader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0/_wheattex->getWidth(),1.0/_wheattex->getHeight());

    // Allocate the vertex buffer (this binds as well)
    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex2));
    _vertbuff->setupAttribute("aPosition", 2, GL_FLOAT, GL_FALSE,
                              offsetof(cugl::SpriteVertex2,position));
    _vertbuff->setupAttribute("aColor",    4, GL_UNSIGNED_BYTE, GL_TRUE,
                              offsetof(cugl::SpriteVertex2,color));
    _vertbuff->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
                              offsetof(cugl::SpriteVertex2,texcoord));


    _vertbuff->bind();
    
    // Cover the entire screen with a rectangle
    SpriteVertex2 vert;
    vert.position = Vec2(0, 0);
    vert.color = Color4::WHITE.getPacked();
    vert.texcoord = Vec2(0, 1);  // Flip the y-coordinate
    _mesh.vertices.push_back(vert);

    vert.position = Vec2(_size.width, 0);
    vert.texcoord = Vec2(1, 1);
    _mesh.vertices.push_back(vert);

    vert.position = Vec2(0, _size.height);
    vert.texcoord = Vec2(0, 0);
    _mesh.vertices.push_back(vert);
    
    vert.position = Vec2(_size.width, _size.height);
    vert.texcoord = Vec2(1, 0);
    _mesh.vertices.push_back(vert);

    _mesh.indices = {0, 1, 2, 0, 1, 2, 3}; // Two? triangles to cover the rectangle

    _mesh.command = GL_TRIANGLES;
    
    // IMPORTANT LAST STEP: Load the mesh into the vertex buffer
    // We only need to reload it if the vertex data changes (which is never)
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
    _vertbuff->unbind();
}
