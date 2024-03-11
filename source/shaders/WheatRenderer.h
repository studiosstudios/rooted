//
//  WheatRenderer.hpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#ifndef WheatRenderer_h
#define WheatRenderer_h

#include <cugl/cugl.h>

class WheatRenderer {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

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
    
    /**
     * Builds the graphics pipeline.
     *
     * This initializes the shader and adds the triangle
     */
    void buildPipeline();
    
public:
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
    WheatRenderer() {}
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits.
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~WheatRenderer() { }
    

    
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
    virtual void draw();
    
};

#endif /* WheatRenderer_h */
