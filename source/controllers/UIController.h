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
#include "../objects/EntityModel.h"


class UIController {
private:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** reference to the input controller */
    std::shared_ptr<InputController> _input;
    
    /** Reference to this player's EntityModel */
    std::shared_ptr<EntityModel> _character;
    
//    /** PolyFactory for drawing the joystick circle.
//     *  TODO: We can remove this once we get proper assets for the joystick
//     */
    cugl::PolyFactory _pf;
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
    
    std::shared_ptr<cugl::scene2::PolygonNode> _dashTimerNode;
    
    /** Pointer to the TextureNode of the top-left CARROT count */
    std::shared_ptr<cugl::scene2::TexturedNode> _carrotsRemainingBoard;
    std::shared_ptr<cugl::scene2::Label> _carrotsRemainingText;
    
    /** Pointer to the TextureNode of the top-right BABY CARROT count */
    std::shared_ptr<cugl::scene2::TexturedNode> _barrotsRemainingBoard;
    std::shared_ptr<cugl::scene2::Label> _barrotsRemainingText;
    
    /** UI elements for the tutorial */
    std::shared_ptr<cugl::scene2::NinePatch> _speechBubble;
    std::shared_ptr<cugl::scene2::Label> _speechBubbleText;
    std::shared_ptr<cugl::scene2::NinePatch> _dialogBox;
    std::shared_ptr<cugl::scene2::Label> _dialogBoxText;

    
    float swipeThickness = 8;
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
    std::shared_ptr<cugl::scene2::Label> _roundNumber;
    std::shared_ptr<cugl::scene2::Label> _time;
    std::shared_ptr<cugl::scene2::Label> _babyCarrots;
    std::vector<std::shared_ptr<cugl::scene2::SceneNode>> _carrotsRooted;
    
    std::shared_ptr<cugl::scene2::Button> _nextbutton;
    std::shared_ptr<cugl::scene2::SceneNode> _playerpointinfo;
    std::shared_ptr<cugl::scene2::Button> _exitbutton;
    std::shared_ptr<cugl::scene2::Button> _nextroundbutton;
    // a map from the little icon of the player to an array of the number of points you have
    std::map<std::shared_ptr<cugl::scene2::SceneNode>, std::vector<std::shared_ptr<cugl::scene2::SceneNode>>> _points;
    
    std::shared_ptr<cugl::Texture> _filledpoint;
    
    bool _nextRound;
    
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
    void setEndVariables(int roundNum, int length, int babies, int carrots, std::vector<int> pointsVec);

    void setSpeechBubbleVisible(bool visible);
    bool getSpeechBubbleVisible() { return _speechBubble->isVisible(); };
    void setDialogBoxVisible(bool visible);
    bool getDialogBoxVisible() { return _dialogBox->isVisible(); };
    void setDialogBoxText(std::string text);
    
    void setCharacter(const std::shared_ptr<EntityModel>& character) {
        _character = character;
    }

    void initJoystickNodes();

    void initGameUINodes();
    
    bool init(const std::shared_ptr<cugl::AssetManager>& assets,
              const std::shared_ptr<InputController>& input,
              const std::shared_ptr<cugl::scene2::SceneNode> uinode,
              cugl::Vec2 offset, float cameraZoom, float drawScale);
    
    void updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick);
    
    std::list<cugl::Vec2> getAdjustedSwipePoints();
    
    std::vector<cugl::Vec2> computeTriangulatedPoints();
    
    std::vector<Uint32> computeTriangulatedIndices(int numTriangles);
    
    cugl::Color4 getSwipeColorForInput();
    
    void updateSwipeSpline();
    
    void updateInfoNodes(int numCarrots, int numBarrots);
    
    void updateDashTimerNode(bool canDash);
    
    void update(float step, std::shared_ptr<cugl::OrthographicCamera> camera, int numCarrots, int numBarrots, bool debugActive, bool canDash);
    
    bool isNextRound() { return _nextRound; }
    
    void setNextRound(bool n) { _nextRound = n; }
};

#endif /* UIController_h */
