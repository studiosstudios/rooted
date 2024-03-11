//
//  WheatRenderer.hpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#ifndef WheatRenderer_h
#define WheatRenderer_h

#include <cugl/cugl.h>
#include "../controllers/CameraController.h"
#include "../objects/Map.h"



class WheatRenderer {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    Size _size;
    
    /** Controller for camera */
    std::shared_ptr<cugl::OrthographicCamera> _cam;

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

    /** The type */
    int _type;
    
    /** The listener key for input */
    Uint32 _key;
    
    
public:
    
    WheatRenderer() {}

    ~WheatRenderer() {}
            
    /**
     * Sets the loaded assets for this game level
     *
     * @param assets the loaded assets for this game level
     */
    void load(const std::shared_ptr<cugl::AssetManager> &assets, const std::shared_ptr<cugl::OrthographicCamera> &camera);
    
    void setSize(Size size) { _size = size; }
        
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

#endif /* WheatRenderer_h */
