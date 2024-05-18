//
//  CameraController.hpp
//  Rooted
//
//  Created by Kimmy Lin on 2/28/24.
//

#ifndef CameraController_hpp
#define CameraController_hpp

#include <cugl/cugl.h>
#include "../objects/EntityModel.h"

using namespace cugl;

/**
* A controller for the perspective in the game. It will follow a
* target at a specific zoom level given the root node of the scene
* and the target node in the scene lazily given a lerp value.
*/
class CameraController {
protected:
    /* The scene position that this camera moves towards */
    Vec2 _posTarget;
    /* The scene position of this camera before any screen shake */
    Vec2 _pos;
    /* The zoom target that this camera interpolates towards */
    float _zoomTarget;
    /* The zoom that this camera should maintain during regular gameplay */
    float _defaultZoom;
    /* The pointer to root node of the scene */
    std::shared_ptr<scene2::SceneNode> _root;
    /* The UI layer that will move with the camera */
    std::shared_ptr<scene2::SceneNode> _ui;
    /* The camera for this scene */
    std::shared_ptr <cugl::OrthographicCamera> _camera;
    /* The lerp speed of the camera */
    float _lerp;
    /* The maximum allowed zoom */
    float _maxZoom;
    /* Draw scale */
    float _scale;
    /* The proportion of the scene that the camera should stay inside */
    Rect _frac;
    /* Amount of time to hold camera in zoom out pos upon cam init */
    int _startTimer;
    /* Max amount to shake the cam per frame */
    float _shake; 
    bool _isTutorial;
    
    bool _nozoom;
    
public:
#pragma mark -
#pragma mark Constructors

    /**
    * Creates a new camera controller with the default values.
    *
    * This constructor does not allocate any objects or start the controller.
    * This allows us to use a controller without a heap pointer.
    */
    CameraController() {};

    /**
    * Destroys the CameraController, releasing all resources.
    */
    virtual ~CameraController(void) {};

    /**
    * Initializes the controller, and starts the camera tracking.
    *
    * @param target     The scene graph object that the camera will target
    * @param root       The root node in our scene graph so the camera can move the view
    * @param lerp       The lerp speed of the camera
    * @param screenSize The width and height of the screen
    *
    * @return true if the controller is initialized properly, false otherwise
    */
    bool init(const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr <cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom, float scale, Rect frac, bool tutorial);
#pragma mark -
#pragma mark Camera Handling

    /**
    * The method called to update the camera
    *
    * This method will move the view towards the target based off of lerp
    *
    * @param dt The delta time
    */
    void update(float dt);

    /**
    * This method sets the zoom of the camera by a multiplier.
    *
    * @param zoom The zoom multiplier
    */
    void setZoom(float zoom);

    /**
    * This method adds the zoom of the camera by a value.
    *
    * @param zoom The zoom value
    */
    void zoomIn(float zoom);

    /**
    * This method sets the position of the camera.
    */
    void setPosition(Vec3 pos) { setPosition(pos, true); }

    void setPosition(Vec3 pos, bool setTarget);

    void setFrac(Rect frac) { _frac = frac; }
    
    void setZoomTarget(float target) { _zoomTarget = target; }
    
    void setTarget(const Vec2 target) { _posTarget = target; }
    
    void setTarget(float x, float y) { _posTarget.set(x, y); }

    void setShake(float shake) { _shake = shake; }
    
    /**
    * Gets the camera
    *
    * @return The camera
    */
    std::shared_ptr<cugl::OrthographicCamera> getCamera() { return _camera; };
    
    const Vec2 boundPosition(Vec2 pos);
    
    void setNoZoom(bool z) { _nozoom = z; }
};

#endif
