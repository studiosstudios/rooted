//
// Created by Jeffrey Zhou on 3/21/24.
//
// This is a simple custom scene2 node that renders our shaders.
// Its only purpose is to manage the draw order of shaders, and thus
// this node does not expect to have any children, and will not
// render them.
//

#ifndef ROOTED_SHADERNODE_H
#define ROOTED_SHADERNODE_H

#include "ShaderRenderer.h"
#include <cugl/cugl.h>

class ShaderNode : public scene2::SceneNode {

public:
    enum class ShaderType : int {
        WHEAT,
        GROUND,
        CLOUDS
    };

protected:
    std::shared_ptr<ShaderRenderer> _renderer;
    ShaderNode::ShaderType _type;

public:

    ShaderNode();

    ~ShaderNode() { dispose(); }

    static std::shared_ptr<ShaderNode> alloc(const std::shared_ptr<ShaderRenderer> &renderer, ShaderType type) {
        std::shared_ptr<ShaderNode> result = std::make_shared<ShaderNode>();
        return (result->init(renderer, type) ? result : nullptr);
    }

    bool init (const std::shared_ptr<ShaderRenderer> &renderer, ShaderType type);

    void dispose() override;

    /**
     * This overrides the Scene2 render and thus does not draw any children
     */
    virtual void draw(const std::shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) override;
};


#endif //ROOTED_SHADERNODE_H
