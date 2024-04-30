//
//  UIController.h
//  Rooted
//
//  Created by Choong Jae Lee on 2/27/24.
//

#ifndef ROOTED_UICONTROLLER_H
#define ROOTED_UICONTROLLER_H

#include <cugl/cugl.h>
#include "../controllers/InputController.h"

#endif /* UIController_h */

class UIController {
private:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** reference to the input controller */
    std::shared_ptr<InputController> _input;
    
//    /** PolyFactory for drawing the joystick circle.
//     *  TODO: We can remove this once we get proper assets for the joystick
//     */
//    cugl::PolyFactory _pf;
//    
//    /** */
//    cugl::Spline2 _spline;
//    cugl::SplinePather _sp;
//    cugl::SimpleExtruder _se;
    std::shared_ptr<cugl::scene2::PolygonNode> _swipeNode;
    
    /** Pointer to the UI root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _uinode;
    
    /** Pointer to the SceneNode which is the root of the joystick UI */
    std::shared_ptr<cugl::scene2::SceneNode> _joynode;
    
    /** Pointer to the PolygonNode of the joystick's backing circle*/
    std::shared_ptr<cugl::scene2::PolygonNode> _joyback;
    /** Pointer to the PolygonNode of the joystick's moving circle*/
    std::shared_ptr<cugl::scene2::PolygonNode> _joymain;
    
    /** Pointer to the TextureNode of the VICTORY! message */
    std::shared_ptr<cugl::scene2::TexturedNode> _winNode;
    /** Pointer to the TextureNode of the DEFEAT. message*/
    std::shared_ptr<cugl::scene2::TexturedNode> _loseNode;
    
    /** Pointer to the TextureNode of the top-left CARROT count */
    std::shared_ptr<cugl::scene2::TexturedNode> _carrotsRemainingBoard;
    std::shared_ptr<cugl::scene2::Label> _carrotsRemainingText;
    
    /** Pointer to the TextureNode of the top-right BABY CARROT count */
    std::shared_ptr<cugl::scene2::TexturedNode> _barrotsRemainingBoard;
    std::shared_ptr<cugl::scene2::Label> _barrotsRemainingText;
    
    float swipeThickness = 8;
    Uint32 swipeDurationMillis = 500;
    cugl::Vec2 tmp;
    std::optional<cugl::Color4> swipeColor;
    
    /** The offset of the scene graph
     *
     *  This offset is nonzero if the device's screen is not 1024 x 576.
     *  In which case, either the X xor Y coordinate will be non-zero.
     */
    cugl::Vec2 _offset;
    
    /** The camera's zoom */
    float _cameraZoom;
    /** The draw scale */
    float _drawScale;
    
    /** The scenes for game over */
    std::shared_ptr<cugl::scene2::SceneNode> _postroundscene;
    std::shared_ptr<cugl::scene2::Button> _nextbutton;
    std::shared_ptr<cugl::scene2::SceneNode> _playerpointinfo;
    std::shared_ptr<cugl::scene2::Button> _nextroundbutton;
    
public:
#pragma mark Constructor/Destructor/Dispose
#pragma mark -
    UIController() {};
    
    ~UIController() { dispose(); }
    
    void dispose();
    
#pragma mark Getters/Setters
#pragma mark -
    void setWinVisible(bool visible);
    void setLoseVisible(bool visible);
    void setEndVisible(bool visible);
    
    void initJoystickNodes();

    void initGameUINodes();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
              const std::shared_ptr<InputController>& input,
              const std::shared_ptr<cugl::scene2::SceneNode> uinode,
              cugl::Vec2 offset, float cameraZoom, float drawScale);
    
    void updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick);
    
    void cullSwipePointsByDuration();
    
    std::list<cugl::Vec2> getAdjustedSwipePoints();
    
    std::vector<cugl::Vec2> computeTriangulatedPoints();
    
    std::vector<Uint32> computeTriangulatedIndices(int numTriangles);
    
    void updateSwipeSpline();
    
    void updateInfoNodes(int numCarrots, int numBarrots);
    
    void update(float step, std::shared_ptr<cugl::OrthographicCamera> camera, int numCarrots, int numBarrots, bool debugActive);
};
