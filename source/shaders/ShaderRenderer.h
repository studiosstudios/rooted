//
//  WheatRenderer.hpp
//  Rooted
//
//  Created by Nicole Thean on 3/11/24.
//

#ifndef WheatRenderer_h
#define WheatRenderer_h

#include <cugl/cugl.h>
#include "../objects/Carrot.h"
#include "../objects/BabyCarrot.h"
#include "../objects/Farmer.h"

class ShaderRenderer {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    Size _size;
    
    float _scale;
    /** A shader to render wheat */
    std::shared_ptr<cugl::Shader> _wheatShader;
    /** A shader to render the ground (i.e. shadows) */
    std::shared_ptr<cugl::Shader> _groundShader;
    /** A shader to render the clouds */
    std::shared_ptr<cugl::Shader> _cloudsShader;
    /** A vertex buffer to receive our triangle */
    std::shared_ptr<cugl::VertexBuffer> _vertbuff;
    /** The mesh for storing the drawing data */
    cugl::Mesh<cugl::SpriteVertex2> _mesh;
    
    float _windTime;
    float _cloudTime;
    
    std::shared_ptr<cugl::Texture> _wheattex;
    std::shared_ptr<cugl::Texture> _noisetex;
    std::shared_ptr<cugl::Texture> _cloudtex;
    std::shared_ptr<cugl::Texture> _gradienttex;
    std::shared_ptr<cugl::Texture> _grassgradienttex;
    std::vector<std::shared_ptr<cugl::Texture>> _textures;
    float _aspectRatio;
    float _bladeColorScale;
    bool _fullHeight;
    
    
public:
    
    ShaderRenderer() {}

    ~ShaderRenderer() {}
    
    static std::shared_ptr<ShaderRenderer> alloc(const std::shared_ptr<Texture> &wheattex, const std::shared_ptr<cugl::AssetManager> &assets,
                                                 std::string name, float bladeColorScale, Size size, bool fullHeight) {
        std::shared_ptr<ShaderRenderer> result = std::make_shared<ShaderRenderer>();
        return (result->init(wheattex, assets, name, bladeColorScale, size, fullHeight) ? result : nullptr);
    }
    
    bool init(const std::shared_ptr<Texture> &wheattex, const std::shared_ptr<cugl::AssetManager> &assets, std::string name, float bladeColorScale, Size size, bool fullHeight);
    
    void setSize(Size size) { _size = size; }
    
    void setScale(float scale) { _scale = scale; }
    
    float getAspectRatio() { return _aspectRatio; }
    
    void setAssets(const std::shared_ptr<cugl::AssetManager> &assets) { _assets = assets; }
        
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
    void update(float timestep, const Mat4& perspective, int size, float *positions, float *velocities, Vec2 playerPos);
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    void renderWheat();

    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    void renderGround();

    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    void renderClouds();
    
    /**
     * Builds the graphics pipeline.
     *
     * This initializes the shaders and adds the triangles
     */
    void buildShaders();
    
};

#endif /* WheatRenderer_h */
