//
//  PFInput.h
//  PlatformDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#include "InputController.h"
#include "../RootedConstants.h"

using namespace cugl;

/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
// Action inputs
_keyDash(false),
_keyDashPressed(false),
_dashPressed(false),

_keyRoot(false),
_rootPressed(false),

_keyUnroot(false),
_unrootPressed(false),

_keyThrowRock(false),
_throwRockPressed(false),

// Meta inputs
_keyReset(false),
_resetPressed(false),
_keyDebug(false),
_debugPressed(false),
_keyExit(false),
_exitPressed(false),
_keyContinue(false),
_keyContinuePressed(false),
_continuePressed(false),

// Utility fields
_active(false),
_dashPressedThisSwipe(false),
_joystick(false),
_paused(false),
_movement(Vec2(0,0)) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeMotionListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
        _swipePoints = nullptr;
        _lineGesturer->dispose();
        _circleGesturer->dispose();
        _lineGesturer = nullptr;
        _circleGesturer = nullptr;
    }
}

/**
 * Initializes the input control for the given bounds
 *
 * The bounds are the bounds of the scene graph.  This is necessary because
 * the bounds of the scene graph do not match the bounds of the display.
 * This allows the input device to do the proper conversion for us.
 *
 * @param bounds    the scene graph bounds
 *
 * @return true if the controller was initialized successfully
 */
bool InputController::init(const Rect bounds, const std::shared_ptr<cugl::JsonValue>& lineData, const std::shared_ptr<cugl::JsonValue>& circleData) {
    bool success = true;
    _time = 0;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    _lineGesturer = cugl::GestureRecognizer::alloc();
    _lineGesturer->setSimilarityThreshold(LINE_GESTURE_SIMILARITY);
    _lineGesturer->setOrientationTolerance(-1);
    _lineGesturer->addGestures(lineData->get("gestures"));

    _circleGesturer = cugl::GestureRecognizer::alloc();
    _circleGesturer->setSimilarityThreshold(CIRCLE_GESTURE_SIMILARITY);
    _circleGesturer->setOrientationTolerance(-1);
    _circleGesturer->addGestures(circleData->get("gestures"));
    _swipePoints = std::make_shared<std::list<std::pair<cugl::Vec2, cugl::Timestamp>>>();
    
    createZones();
    clearTouchInstance(_jtouch);
    clearTouchInstance(_rtouch);
    clearTouchInstance(_mtouch);
    
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
	
#endif
    _active = success;
    _lastAcceleration = 0;
    return success;
}


/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void InputController::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY) && debug;
    _keyExit   = keys->keyPressed(EXIT_KEY);
    _keyDash   = keys->keyPressed(KeyCode::X) && !_paused;
    _keyRoot   = keys->keyPressed(KeyCode::Z) && !_paused;
    _keyUnroot = keys->keyPressed(KeyCode::Z) && !_paused;
    _keyContinue = keys->keyPressed(KeyCode::Q);
    _deviceShaking = keys->keyPressed(KeyCode::A) && !_paused;
    _keyRoot   = keys->keyPressed(KeyCode::Z);
    _keyUnroot = keys->keyPressed(KeyCode::Z);
    _keyThrowRock = keys->keyPressed(KeyCode::SPACE) && !_paused;

    if (keys->keyDown(KeyCode::ARROW_LEFT)) {
        _movement.x = -1.0f;
    } else if (keys->keyDown(KeyCode::ARROW_RIGHT)) {
        _movement.x = 1.0f;
    } else {
        _movement.x = 0;
    }
    
    if (keys->keyDown(KeyCode::ARROW_UP)) {
        _movement.y = 1.0f;
    } else if (keys->keyDown(KeyCode::ARROW_DOWN)) {
        _movement.y = -1.0f;
    } else {
        _movement.y = 0;
    }
    
    if (_keyDash) _dashVector = Vec2(_movement);
    
    _movement *= (1 - _paused);
    _movement.normalize();
#endif
    Accelerometer* acc = Input::get<Accelerometer>();
    if(acc != nullptr && !_paused) {
        float currAcceleration = acc->getAcceleration().lengthSquared();
        float delta = currAcceleration - _lastAcceleration;
        _acceleration = _acceleration * 0.9 + delta;
        _deviceShaking = _acceleration > 0.5;
        _lastAcceleration = currAcceleration;
//        std::cout << "acceleration: " << _acceleration << "\n";
    }
    _resetPressed = _keyReset && debug;
    _debugPressed = _keyDebug && debug;
    _exitPressed  = _keyExit;

    _dashPressed  = (_keyDash && !_keyDashPressed);
    _keyDashPressed = _keyDash;
    
    _rootPressed = _keyRoot;
    
    _unrootPressed = _keyUnroot;

    _continuePressed  = (_keyContinue && !_keyContinuePressed);
    _keyContinuePressed = _keyContinue;
    _throwRockPressed  = _keyThrowRock;
    _keyThrowRock = false;
    _time += dt;

    // _movement is now updated directly in processJoystick

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyExit = false;
    _keyReset = false;
    _keyDebug = false;
    _keyContinue = false;
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    _dashPressed  = false;
    _rootPressed = false;
    _continuePressed = false;
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void InputController::createZones() {
	_jzone = _tbounds;
	_jzone.size.width *= JOY_ZONE_WIDTH;
	_rzone = _tbounds;
	_rzone.size.width *= RIGHT_ZONE;
	_rzone.origin.x = _tbounds.origin.x+_tbounds.size.width-_rzone.size.width;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void InputController::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
}


/**
 * Returns the correct zone for the given position.
 *f
 * See the comments above for a description of how zones work.
 *
 * @param  pos  a position in screen coordinates
 *
 * @return the correct zone for the given position.
 */
InputController::Zone InputController::getZone(const Vec2 pos) const {
	if (_jzone.contains(pos)) {
		return Zone::JOY;
	} else if (_rzone.contains(pos)) {
		return Zone::RIGHT;
	} else if (_tbounds.contains(pos)) {
		return Zone::MAIN;
	}
	return Zone::UNDEFINED;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 InputController::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}



/**
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void InputController::processJoystick(const cugl::Vec2 pos) {
    Vec2 diff =  pos - _jtouch.position;
    _joyCenter = touch2Screen(pos);

    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
        diff.normalize();
    }
    else {
        diff.divide(JSTICK_RADIUS);
    }
    diff.y *= -1;
    
    _movement.set((1 - _paused) * diff);
}

void InputController::showDisplayJoystick() {
    _joystick = true;
    _joyCenter = touch2Screen(cugl::Vec2(SCENE_WIDTH / 3, SCENE_HEIGHT));
    _joyAnchor = touch2Screen(cugl::Vec2(SCENE_WIDTH / 3, SCENE_HEIGHT));
}

/**
 * Returns a nonzero value if this is a quick left or right swipe
 *
 * The function returns -1 if it is left swipe and 1 if it is a right swipe.
 *
 * @param  start    the start position of the candidate swipe
 * @param  stop     the end position of the candidate swipe
 * @param  current  the current timestamp of the gesture
 *
 * @return a nonzero value if this is a quick left or right swipe
 */
int InputController::processSwipe(const Vec2 start, const Vec2 stop, Timestamp current) {
	// Look for swipes up that are "long enough"
	float xdiff = (stop.x-start.x);
    float thresh = SWIPE_LENGTH;
    if (xdiff > thresh) {
        return 1;
    } else if (xdiff < thresh * -1) {
        return -1;
    }
	return 0;
}



#pragma mark -
#pragma mark Touch and Mouse Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    //CULog("Touch began %lld", event.touch);
    Vec2 pos = event.position;
    Vec2 screenPos = touch2Screen(pos);
    Zone zone = getZone(pos);
    _time = 0;
    switch (zone) {
        case Zone::JOY:
            // Only process if no touch in zone
            if (_jtouch.touchids.empty()) {
                // Left is the floating joystick
                _jtouch.position = event.position;
                _jtouch.timestamp.mark();
                _jtouch.touchids.insert(event.touch);

                _joyCenter = screenPos;
                _joyAnchor = _joyCenter;
                _joystick = !_paused;
            }
            break;
        case Zone::RIGHT:
            if ( _rtouch.touchids.empty() && _mtouch.touchids.empty()) {
                _keyThrowRock = (event.timestamp.ellapsedMillis(_rtime) <= DOUBLE_CLICK);
            }
        case Zone::MAIN:
            // Only process if no touch in zone
            if (_rtouch.touchids.empty()) {
                _rtouch.position = event.position;
                _rtouch.timestamp.mark();
                _rtouch.touchids.insert(event.touch);
                _keyDash = false;
                _keyRoot = false;
                _dashPressedThisSwipe = false;
                _swipePoints->clear();
                addSwipePoint(screenPos);
            } else {
                Vec2 offset = event.position-_rtouch.position;
                if (offset.lengthSquared() < NEAR_TOUCH*NEAR_TOUCH) {
                    _rtouch.touchids.insert(event.touch);
                }
            }
            break;
        default:
            CUAssertLog(false, "Touch is out of bounds");
            break;
    }
}

 
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    Vec2 pos = event.position;
    Zone zone = getZone(pos);
    if (_jtouch.touchids.find(event.touch) != _jtouch.touchids.end()) {
        _jtouch.touchids.clear();
        _movement.setZero();
        _joystick = false;
    }
    else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        _keyDash = false;
        _keyRoot = false;
        _keyUnroot = false;
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
    }
    _keyContinue = _time < 0.125;
}


/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus	Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    Vec2 screenPos = touch2Screen(pos);
    // Only check for swipes in the main zone if there is more than one finger.
    if (_jtouch.touchids.find(event.touch) != _jtouch.touchids.end()) {
        processJoystick(pos);
    }
    else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        addSwipePoint(screenPos);
        
        // If this swipe was already used for a dash, can't use it for any other gestures
        if (!_keyDash) {
            // Handle circle (root/unroot gesture)
            bool validCircleGesture = (_swipePoints->size() > SWIPE_POINT_MINIMUM && _circleGesturer->match(getSwipePointsVector()).length() > 0);
            _keyRoot = validCircleGesture;
            _keyUnroot = validCircleGesture;
            
            // Handle dash
            if (_internalSwipePoints.size() > SWIPE_POINT_MINIMUM &&
                _lineGesturer->similarity("line", getInternalSwipePointsVector(), true) > LINE_GESTURE_SIMILARITY &&
                _internalSwipePoints.front().first.distanceSquared(_internalSwipePoints.back().first) > SWIPE_LENGTH * SWIPE_LENGTH){
                _keyDash = !_paused;
                _dashPressedThisSwipe = true;
                loadDashVector();
            }
        }
    }
}

int InputController::signum(int num) {
    if (num > 0) {
        return 1;
    }
    else if (num < 0) {
        return -1;
    }
    else {
        return 0;
    }
}
