//
//  WheatRenderer.hpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#ifndef GroundRenderer_h
#define GroundRenderer_h

#include <cugl/cugl.h>
#include "../controllers/CameraController.h"
#include "../objects/Map.h"



class GroundRenderer {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    Size _size;
    
    /** Controller for camera */
    std::shared_ptr<cugl::OrthographicCamera> _cam;
    
    std::shared_ptr<Map> _map;
    
    float _scale;

//    /** The OpenGL camera */
//    std::shared_ptr<cugl::OrthographicCamera> _camera;
    /** A shader to render our triangle */
    std::shared_ptr<cugl::Shader> _shader;
    /** A vertex buffer to receive our triangle */
    std::shared_ptr<cugl::VertexBuffer> _vertbuff;
    /** The mesh for storing the drawing data */
    cugl::Mesh<cugl::SpriteVertex2> _mesh;
    /** The first texture to try */
    std::shared_ptr<cugl::Texture> _texture1;
    /** The second texture to try */
    std::shared_ptr<cugl::Texture> _texture2;
    
    float _totalTime;
    std::shared_ptr<cugl::Texture> _grasstex;
    std::shared_ptr<cugl::Texture> _noisetex;
    std::shared_ptr<cugl::Texture> _cloudtex;
    std::shared_ptr<cugl::Texture> _gradienttex;
    std::shared_ptr<cugl::Texture> _wheatdetails;
    std::vector<std::shared_ptr<cugl::Texture>> _textures;

    
    
public:
    
    GroundRenderer() {}

    ~GroundRenderer() {}
    
    static std::shared_ptr<GroundRenderer> alloc(const std::shared_ptr<cugl::AssetManager> &assets, const std::shared_ptr<cugl::OrthographicCamera> &camera,
        std::shared_ptr<Map> &map, float scale) {
        std::shared_ptr<GroundRenderer> result = std::make_shared<GroundRenderer>();
        return (result->init(assets, camera, map, scale) ? result : nullptr);
    }
    
    bool init(const std::shared_ptr<cugl::AssetManager> &assets, const std::shared_ptr<cugl::OrthographicCamera> &camera,
        std::shared_ptr<Map> &map, float _scale);
    
    void setSize(Size size) { _size = size; }
    
    void setScale(float scale) { _scale = scale; }
    
    void setAssets(const std::shared_ptr<cugl::AssetManager> &assets) { _assets = assets; }
    
    void setCamera(const std::shared_ptr<cugl::OrthographicCamera> &camera) { _cam = camera; }
        
    void dispose();
    

    
    /**
     * The method called to update the shader data.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should contain any code that is not an OpenGL call.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    virtual void update(float timestep);
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void render();
    
    /**
     * Builds the graphics pipeline.
     *
     * This initializes the shader and adds the triangle
     */
    void buildShader();
    
};

#endif /* GroundRenderer_h */