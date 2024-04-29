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
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_rustlePressed(false),
_dashPressed(false),
_showPlayerPressed(false),
_switchPressed(false),
_rootPressed(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_keyRustle(false),
_keyShowPlayer(false),
_movement(Vec2(0,0)),
_joystick(false),
_hasJumped(false),
_keyDash(false),
_keyDashPressed(false),
_currentSwipeColor(Color4::WHITE) {
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
bool InputController::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    _lineGesturer = cugl::GestureRecognizer::alloc();
    _lineGesturer->setOrientationTolerance(-1);
    std::vector<Vec2> linegestvec {Vec2(-124,0),
        Vec2(-121,0),
        Vec2(-117,0),
        Vec2(-113,0),
        Vec2(-109,0),
        Vec2(-106,0),
        Vec2(-102,0),
        Vec2(-98,0),
        Vec2(-94,0),
        Vec2(-90,0),
        Vec2(-86,0),
        Vec2(-82,0),
        Vec2(-78,0),
        Vec2(-74,0),
        Vec2(-70,0),
        Vec2(-66,0),
        Vec2(-62,0),
        Vec2(-58,0),
        Vec2(-54,0),
        Vec2(-50,0),
        Vec2(-46,0),
        Vec2(-42,0),
        Vec2(-38,0),
        Vec2(-34,0),
        Vec2(-30,0),
        Vec2(-26,0),
        Vec2(-22,0),
        Vec2(-18,0),
        Vec2(-14,0),
        Vec2(-10,0),
        Vec2(-6,0),
        Vec2(-2,0),
        Vec2(2,0),
        Vec2(6,0),
        Vec2(10,0),
        Vec2(14,0),
        Vec2(18,0),
        Vec2(22,0),
        Vec2(26,0),
        Vec2(30,0),
        Vec2(34,0),
        Vec2(38,0),
        Vec2(42,0),
        Vec2(46,0),
        Vec2(50,0),
        Vec2(54,0),
        Vec2(58,0),
        Vec2(62,0),
        Vec2(66,0),
        Vec2(70,0),
        Vec2(74,0),
        Vec2(78,0),
        Vec2(82,0),
        Vec2(86,0),
        Vec2(90,0),
        Vec2(94,0),
        Vec2(98,0),
        Vec2(102,0),
        Vec2(106,0),
        Vec2(110,0),
        Vec2(114,0),
        Vec2(118,0),
        Vec2(122,0),
        Vec2(126,0),
};
    _lineGesturer->addGesture("line", linegestvec);
    _circleGesturer = cugl::GestureRecognizer::alloc();
    _circleGesturer->setSimilarityThreshold(CIRCLE_GESTURE_SIMILARITY);
    _circleGesturer->addGesture("circle", std::vector<cugl::Vec2>{Vec2(-151,0),
        Vec2(-149,9),
        Vec2(-147,19),
        Vec2(-143,27),
        Vec2(-138,36),
        Vec2(-133,44),
        Vec2(-128,53),
        Vec2(-123,61),
        Vec2(-115,68),
        Vec2(-107,74),
        Vec2(-98,80),
        Vec2(-90,86),
        Vec2(-81,92),
        Vec2(-72,96),
        Vec2(-62,100),
        Vec2(-52,104),
        Vec2(-42,108),
        Vec2(-32,111),
        Vec2(-21,112),
        Vec2(-10,113),
        Vec2(0,113),
        Vec2(11,113),
        Vec2(21,109),
        Vec2(31,106),
        Vec2(41,102),
        Vec2(50,97),
        Vec2(56,89),
        Vec2(63,82),
        Vec2(69,74),
        Vec2(76,66),
        Vec2(80,58),
        Vec2(83,49),
        Vec2(87,40),
        Vec2(91,31),
        Vec2(94,22),
        Vec2(95,12),
        Vec2(96,3),
        Vec2(97,-7),
        Vec2(99,-16),
        Vec2(98,-25),
        Vec2(96,-35),
        Vec2(94,-44),
        Vec2(92,-53),
        Vec2(90,-63),
        Vec2(85,-71),
        Vec2(80,-80),
        Vec2(75,-88),
        Vec2(70,-97),
        Vec2(63,-104),
        Vec2(55,-110),
        Vec2(47,-116),
        Vec2(38,-122),
        Vec2(29,-127),
        Vec2(19,-130),
        Vec2(9,-133),
        Vec2(-2,-137),
        Vec2(-12,-135),
        Vec2(-23,-133),
        Vec2(-33,-131),
        Vec2(-44,-128),
        Vec2(-54,-124),
        Vec2(-63,-120),
        Vec2(-73,-116),
        Vec2(-83,-111)});
    _circleGesturer->addGesture("circle2", std::vector<cugl::Vec2>{
        Vec2(-155,0),
        Vec2(-151,-9),
        Vec2(-148,-18),
        Vec2(-144,-26),
        Vec2(-140,-35),
        Vec2(-135,-43),
        Vec2(-129,-51),
        Vec2(-124,-58),
        Vec2(-119,-66),
        Vec2(-112,-73),
        Vec2(-105,-79),
        Vec2(-98,-86),
        Vec2(-91,-92),
        Vec2(-84,-98),
        Vec2(-75,-102),
        Vec2(-66,-105),
        Vec2(-57,-108),
        Vec2(-49,-111),
        Vec2(-39,-112),
        Vec2(-30,-110),
        Vec2(-21,-108),
        Vec2(-12,-106),
        Vec2(-3,-104),
        Vec2(5,-99),
        Vec2(13,-94),
        Vec2(22,-89),
        Vec2(30,-84),
        Vec2(37,-78),
        Vec2(43,-71),
        Vec2(49,-64),
        Vec2(55,-57),
        Vec2(61,-49),
        Vec2(66,-41),
        Vec2(70,-33),
        Vec2(74,-24),
        Vec2(78,-15),
        Vec2(82,-7),
        Vec2(85,2),
        Vec2(87,11),
        Vec2(89,21),
        Vec2(90,30),
        Vec2(92,40),
        Vec2(94,49),
        Vec2(95,58),
        Vec2(93,68),
        Vec2(91,77),
        Vec2(88,86),
        Vec2(86,95),
        Vec2(83,104),
        Vec2(77,112),
        Vec2(71,119),
        Vec2(66,127),
        Vec2(58,131),
        Vec2(49,134),
        Vec2(40,137),
        Vec2(31,138),
        Vec2(22,135),
        Vec2(13,132),
        Vec2(4,130),
        Vec2(-4,125),
        Vec2(-12,119),
        Vec2(-20,114),
        Vec2(-28,109),
        Vec2(-36,104)});
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
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    _keyRustle = keys->keyPressed(KeyCode::M);
    _keyShowPlayer = keys->keyPressed(KeyCode::V);
    _keyDash   = keys->keyPressed(KeyCode::X);
    _keySwitch = keys->keyPressed(KeyCode::S);
    _keyRoot   = keys->keyPressed(KeyCode::Z);
    _keyUnroot = keys->keyPressed(KeyCode::Z);

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
    _movement.normalize();
#endif
    Accelerometer* acc = Input::get<Accelerometer>();
    if(acc != nullptr)
        _deviceShaking = acc->getAcceleration().lengthSquared() > 12;
    else{
        _deviceShaking = false;
    }
    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    // for testing rustling
    _rustlePressed = _keyRustle;
    _showPlayerPressed = _keyShowPlayer;
    
    _switchPressed = _keySwitch;

    _dashPressed  = (_keyDash && !_keyDashPressed);
    _keyDashPressed = _keyDash;
    
    _rootPressed = _keyRoot;
    
    _unrootPressed = _keyUnroot;

    // _movement is now updated directly in processJoystick

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyExit = false;
    _keyReset = false;
    _keyDebug = false;
    _keyShowPlayer = false;
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
    _keyShowPlayer = false;
    _rootPressed = false;
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void InputController::createZones() {
	_jzone = _tbounds;
	_jzone.size.width *= JOY_ZONE_WIDTH;
    _jzone.size.height *= JOY_ZONE_HEIGHT;
    _jzone.origin.y += _jzone.size.height; // We add to the origin because this is when coordinates still have y-origin in top-left
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
    
    _movement.set(diff);
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
                _joystick = true;
            }
            break;
        case Zone::RIGHT:
        case Zone::MAIN:
            // Only process if no touch in zone
            if (_rtouch.touchids.empty()) {
                _rtouch.position = event.position;
                _rtouch.timestamp.mark();
                _rtouch.touchids.insert(event.touch);
                _keyDash = false;
                _keyRoot = false;
                _keyShowPlayer = false;
                _swipePoints->clear();
                _currentSwipeColor = Color4::WHITE;
                addSwipePoint(screenPos);
            }
            break;
//        case Zone::MAIN:
            // Only check for double tap in Main if nothing else down
            
            
//            if (_jtouch.touchids.empty() && _rtouch.touchids.empty() && _mtouch.touchids.empty()) {
//                _keyDebug = (event.timestamp.ellapsedMillis(_mtime) <= DOUBLE_CLICK);
//            }
//            
//            // Keep count of touches in Main zone if next to each other.
//            if (_mtouch.touchids.empty()) {
//                _mtouch.position = event.position;
//                _mtouch.touchids.insert(event.touch);
//            } else {
//                Vec2 offset = event.position-_mtouch.position;
//                if (offset.lengthSquared() < NEAR_TOUCH*NEAR_TOUCH) {
//                    _mtouch.touchids.insert(event.touch);
//                }
//            }
            
            
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
    // Reset all keys that might have been set
    Vec2 pos = event.position;
//    Vec2 screenPos = touch2Screen(pos);
    Zone zone = getZone(pos);
    if (_jtouch.touchids.find(event.touch) != _jtouch.touchids.end()) {
        _jtouch.touchids.clear();
        _movement.setZero();
        _joystick = false;
    }
    else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        _keyDash = false;
//        _keySwitch = false;
        _keyRoot = false;
        _keyUnroot = false;
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
    }
    else if (zone == Zone::MAIN) {
        if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
            _mtouch.touchids.erase(event.touch);
        }
        _mtime = event.timestamp;
    }
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
//                std::cout << "Circle CCW: " << _gesturer->similarity("circle", getInternalSwipePointsVector(), true) << "\n";
//                std::cout << "Circle CW: " << _gesturer->similarity("circle2", getInternalSwipePointsVector(), true) << "\n";
                _keyDash = true;
                _currentSwipeColor = Color4::ORANGE;
            }
                
                //            if (_swipePoints->begin() != _swipePoints->end() && (screenPos.y - _swipePoints->back().first.y) > SWIPE_LENGTH) {
                //                _keyDash = true;
                //                _currentSwipeColor = Color4::ORANGE;
                //            }
                //            else if (_swipePoints->begin() != _swipePoints->end() && (_swipePoints->back().first.y - screenPos.y) > SWIPE_LENGTH) {
                //                _keyRoot = true;
                //                _keyUnroot = true;
                //                _rtouch.position = pos;
                //                _currentSwipeColor = Color4::BLUE;
                //            }
                //            else if ((pos.x-_rtouch.position.x) > SWIPE_LENGTH) {
                //                _keyShowPlayer = true;
                //            }
            }
        }
    }
//    else if (_mtouch.touchids.size() > 1) {
//        // We only process multifinger swipes in main
//        int swipe = processSwipe(_mtouch.position, event.position, event.timestamp);
//        if (swipe == 1) {
//            _keyReset = true;
//        }
//    }

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
