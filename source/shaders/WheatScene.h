// This is a Scene2Texture that represents the wheat texture given to the shaders in ShaderRenderer
// and EntitiesNode. This allows for a much easier way to adjust wheat heights during gameplay that
// does not involve a lot of for-loops in the shaders themselves.
//
// For a given pixel, the shaders read the value in the red channel as the base height of the wheat,
// the value in the green channel as the height to increase by, and the height in the blue channel
// as the height to decrease by. For example if a given pixel has color (10, 4, 3) then the wheat
// height represented there is 11. Therefore to adjust wheat heights you need to add SceneNodes to
// the root node with corresponding colors as desired. To ensure that colors are blending properly
// you MUST call setBlendFunc(GL_DST_ALPHA, GL_ZERO, GL_ONE, GL_ONE) on the node. If you want to
// just directly set the wheat height (i.e. not adding or subtracting) then you should draw to the
// red channel with regular blending.
//
// Note that the root node of this scene is in physics coordinates, so any textures added to this
// scene graph need to be scaled down by the draw scale. This is for convenience purposes
// so that other classes are unaware of the size of the wheat texture. However I am not sure if this
// could become an issue with scaling down textures.
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
    /** the root node of the wheat scene */
    shared_ptr<scene2::SceneNode> _rootnode;
    /** color scale of wheat texture */
    float _bladeColorScale;
    /** a full screen quad shader for debug rendering the full wheat texture */
    shared_ptr<Shader> _fsqshader;
    /** the base wheat texture */
    shared_ptr<Texture> _wheattex;

public:

    WheatScene() {};

    ~WheatScene() { dispose(); };

    static shared_ptr<WheatScene> alloc(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale, Vec2 drawScale, Size worldSize) {
        std::shared_ptr<WheatScene> result = std::make_shared<WheatScene>();
        return (result->init(assets, name, bladeColorScale, drawScale, worldSize) ? result : nullptr);
    }

    bool init(const shared_ptr<AssetManager> &assets, string name, float bladeColorScale, Vec2 drawScale, Size worldSize);

    void dispose();

    /**
     * Renders the full wheat scene texture to the screen. This should only be used for debugging
     * purposes.
     *
     * @param alpha     transparency of the texture. default value is 1.0
     * @param scale     how much to scale the rgb channels by. default value is 8.5 so max wheat
     *                  height of 30 corresponds to 255
     */
    void renderToScreen(float alpha = 1.0, float scale = 8.5);

    shared_ptr<scene2::SceneNode> getRoot() { return _rootnode; }

};


#endif //ROOTED_WHEATSCENE_H
