//
//  WheatRenderer.cpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#include "WheatRenderer.h"

using namespace cugl;

#pragma mark -
#pragma mark Asset Constants
# define BASE_TEXTURE       "shader_base"
/** The key for the earth texture in the asset manager */
#define CLOUD_TEXTURE       "shader_clouds"
/** The name of a wall (for object identification) */
#define NOISE_TEXTURE       "shader_noise"
/** The name of a platform (for object identification) */
#define GRADIENT_TEXTURE    "shader_gradient"

/**
 * Default fragment shader
 *
 * This trick uses C++11 raw string literals to put the shader in a separate
 * file without having to guarantee its presence in the asset directory.
 * However, to work properly, the #include statement below MUST be on its
 * own separate line.
 */
const std::string oglShaderFrag =
#include "fragment.frag"
;

/**
 * Default vertex shader
 *
 * This trick uses C++11 raw string literals to put the shader in a separate
 * file without having to guarantee its presence in the asset directory.
 * However, to work properly, the #include statement below MUST be on its
 * own separate line.
 */
const std::string oglShaderVert =
#include "vertex.vert"
;


bool WheatRenderer::init(const std::shared_ptr<cugl::AssetManager> &assets, const std::shared_ptr<cugl::OrthographicCamera> &camera,
                         std::shared_ptr<Map> &map, float scale) {
    
    
    _size = Application::get()->getDisplaySize();
    _assets = assets;
    _cam = camera;
    _map = map;
    _scale = scale;
    
    _totalTime = 0;
    
    _grasstex = _assets->get<Texture>("shader_base");
    _cloudtex = _assets->get<Texture>("shader_clouds");
    _noisetex = _assets->get<Texture>("shader_noise");
    _gradienttex = _assets->get<Texture>("shader_gradient");
    
    _textures.push_back(_grasstex);
    _textures.push_back(_cloudtex);
    _textures.push_back(_noisetex);
    _textures.push_back(_gradienttex);
    
    for (int i = 0; i < _textures.size(); i++) {
        _textures[i]->setBindPoint(i);
    }
    
    return true;
    
}

void WheatRenderer::dispose() {
    _shader = nullptr;
    _vertbuff = nullptr;
}

void WheatRenderer::update(float timestep) {
    if (_shader) {
        _shader->bind();
        _totalTime += timestep;
        _shader->setUniform1f("TIME", _totalTime);
        _shader->setUniformMat4("uPerspective", _cam->getCombined());
        _shader->setUniform2f("cam_pos", _map->getCarrots().at(0)->getX()/_scale, 1 - _map->getCarrots().at(0)->getY()/_scale);
        _shader->setUniform2f("cam_vel", _map->getCarrots().at(0)->getVX(), _map->getCarrots().at(0)->getVY());
        _shader->unbind();
    }
}

void WheatRenderer::render() {
    // OpenGL commands to enable alpha blending (if needed)
//    _shader->bind();
    
    if (_shader) {
        _vertbuff->attach(_shader);
        
        // Draw with the given textures (if it exists)
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

void WheatRenderer::buildShader() {

    
    CULog("scale: %f \t size: (%f, %f)", _scale, _size.width, _size.height);
    
    
    // Allocate the shader (this binds as well)
    _shader = Shader::alloc(SHADER(oglShaderVert), SHADER(oglShaderFrag));
    
    GLenum error = glGetError();
    if (error) {
        CULog("ERROR 1: %s",gl_error_name(error).c_str());
    }
    
    // Attach the camera to the shader
    _shader->setUniformMat4("uPerspective",_cam->getCombined());
    
    _shader->setSampler("grass_tex", _grasstex);
    _shader->setSampler("cloud_tex", _cloudtex);
    _shader->setSampler("noise_tex", _noisetex);
    _shader->setSampler("gradient_tex", _gradienttex);
    _shader->setSampler("wheat_details_tex", _wheatdetails);

    // Allocate the vertex buffer (this binds as well)
    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex2));
    _vertbuff->setupAttribute("aPosition", 2, GL_FLOAT, GL_FALSE,
                              offsetof(cugl::SpriteVertex2,position));
    _vertbuff->setupAttribute("aColor",    4, GL_UNSIGNED_BYTE, GL_TRUE,
                              offsetof(cugl::SpriteVertex2,color));
    _vertbuff->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
                              offsetof(cugl::SpriteVertex2,texcoord));
    
    // Attach the shader
    _vertbuff->attach(_shader);
    
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
    
    _shader->setUniform1f("TIME", _totalTime);
    _shader->setUniform4f("tip_color", 0.996078, 0.976471, 0.517647, 1.0);
    _shader->setUniform4f("wind_color", 1.0, 0.984314, 0.639216, 1.0);
    _shader->setUniform1f("wind_speed", 1.0);
    _shader->setUniform1f("cloud_speed", 0.05);
    _shader->setUniform2f("wind_direction", 1.0, 1.0);
    _shader->setUniform2f("noise_tex_size", 50.0, 1.0);
    _shader->setUniform2f("SCREEN_PIXEL_SIZE", 1.0/_grasstex->getWidth(),1.0/_grasstex->getHeight());
    
    // IMPORTANT LAST STEP: Load the mesh into the vertex buffer
    // We only need to reload it if the vertex data changes (which is never)
    _vertbuff->loadVertexData(_mesh.vertices.data(), (int)_mesh.vertices.size());
    _vertbuff->loadIndexData(_mesh.indices.data(), (int)_mesh.indices.size());
}
