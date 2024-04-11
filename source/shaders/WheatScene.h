//
// Created by Jeffrey Zhou on 4/10/24.
//

#ifndef ROOTED_WHEATSCENE_H
#define ROOTED_WHEATSCENE_H
#include <cugl/cugl.h>

using namespace cugl;
using namespace std;

class WheatScene : public Scene2Texture {
private:
    shared_ptr<scene2::SceneNode> _rootnode;
    float _bladeColorScale;
    
public:
    shared_ptr<Texture> _wheattex;

    WheatScene() {};

    ~WheatScene() { dispose(); };

    static shared_ptr<WheatScene> alloc(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale) {
        std::shared_ptr<WheatScene> result = std::make_shared<WheatScene>();
        return (result->init(assets, name, bladeColorScale) ? result : nullptr);
    }

    bool init(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale);

    void dispose();
    
    void bindTexture(int bindpoint);
    
    void unbindTexture();

    void renderToTarget(const shared_ptr<SpriteBatch> &batch);
    
    /** FOR DEBUGGING ONLY */
    void drawRenderTarget(const shared_ptr<SpriteBatch> &batch);

};


#endif //ROOTED_WHEATSCENE_H
