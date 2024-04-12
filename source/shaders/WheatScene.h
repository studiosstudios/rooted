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
    shared_ptr<Shader> _fsqshader;
    PolyFactory _pf;
    
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

    void renderToScreen(float alpha, float scale);
    
    void renderToScreen() { renderToScreen(1.0, 8.0); };

    void renderToScreen(float alpha) { renderToScreen(alpha, 8.0); };

    shared_ptr<scene2::SceneNode> getRoot() { return _rootnode; }

};


#endif //ROOTED_WHEATSCENE_H
