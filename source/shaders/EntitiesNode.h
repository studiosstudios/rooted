//
// Created by Jeffrey Zhou on 3/27/24.
//

#ifndef ROOTED_ENTITIESNODE_H
#define ROOTED_ENTITIESNODE_H

#include <cugl/cugl.h>
#include "../objects/BabyCarrot.h"
#include "../objects/Carrot.h"
#include "../objects/Farmer.h"

using namespace std;

using namespace cugl;

class EntitiesNode : public scene2::SceneNode {
private:
    shared_ptr<Shader> _coverShader;
    shared_ptr<scene2::SceneNode> _root;
    shared_ptr<Texture> _wheattex;
    float _windTime;
    shared_ptr<Texture> _noisetex;
    bool _fullHeight;

public:

    EntitiesNode ();

    ~EntitiesNode();

    static std::shared_ptr<EntitiesNode> alloc(const std::shared_ptr<scene2::SceneNode> &node, const shared_ptr<Texture> &wheattex,
                                               const std::shared_ptr<cugl::AssetManager> &assets, bool fullHeight) {
        shared_ptr<EntitiesNode> result = make_shared<EntitiesNode>();
        return (result->init(node, wheattex, assets, fullHeight) ? result : nullptr);
    }

    void update(float timestep);

    void clearNode();

    bool init(const std::shared_ptr<scene2::SceneNode> &node, const shared_ptr<Texture> &wheattex,
              const std::shared_ptr<cugl::AssetManager> &assets,bool fullHeight);

    void dispose() override;

    void draw(const shared_ptr<SpriteBatch> &batch, const cugl::Affine2 &transform, cugl::Color4 tint) override;

};


#endif //ROOTED_ENTITIESNODE_H
