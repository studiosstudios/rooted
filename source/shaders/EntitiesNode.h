//
// Created by Jeffrey Zhou on 3/27/24.
//

#ifndef ROOTED_ENTITIESNODE_H
#define ROOTED_ENTITIESNODE_H

#include <cugl/cugl.h>
#include "BabyCarrot.h"
#include "Carrot.h"
#include "Farmer.h"

using namespace std;

using namespace cugl;

class EntitiesNode : public scene2::SceneNode {
private:
    shared_ptr<RenderTarget> _renderTarget;
    shared_ptr<Shader> _coverShader;
    shared_ptr<scene2::SceneNode> _root;
    shared_ptr<TextureRenderer> _renderer;

public:

    EntitiesNode ();

    ~EntitiesNode();

    static std::shared_ptr<EntitiesNode> alloc(int width, int height) {
        shared_ptr<EntitiesNode> result = make_shared<EntitiesNode>();
        return (result->init(width, height) ? result : nullptr);
    }

    void allocNode();

    void clearNode();

    void addEntityNode(const shared_ptr<SceneNode>& entityNode);

    bool init(int width, int height);

    void dispose() override;

    void draw(const shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) override;

};


#endif //ROOTED_ENTITIESNODE_H
