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
#ifndef __PF_INPUT_H__
#define __PF_INPUT_H__
#include <cugl/cugl.h>
#include <unordered_set>
#include "../RootedConstants.h"

/**
 * This class represents player input in the Platform demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * In the case of touch devices, this implements the platformer controls
 * found in the popular game Oddmar.  The left side is a floating joystick
 * for left-right controls.  The right side is swipe up to jump and double
 * tap to shoot.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class InputController {
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    /** Whether the dash key is down */
    bool _keyDash;
    /** Whether the root key is down*/
    bool _keyRoot;
    /** Whether the unroot key is down*/
    bool _keyUnroot;
    
    /** Whether the dash key was pressed in previous frame */
    bool _keyDashPressed;
    
    // FOR TESTING RUSTLING
    bool _keyRustle;
    bool _keyShowPlayer;
    
    bool _keySwitch;
  
protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** Whether the dash action was chosen. */
    bool _dashPressed;
    /** Whether the dash action was chosen within the last swipe instance. Set to true when dash is true, and set to false on a new swipe instance.*/
    bool _dashPressedThisSwipe;
    /** Whether the root action was chosen */
    bool _rootPressed;
    /** Whether the unroot action was chosen */
    bool _unrootPressed;
    /** Movement vector */
    cugl::Vec2 _movement;
    
    // CAN REMOVE THIS, THIS IS JUST FOR TESTING
    bool _rustlePressed;
    bool _showPlayerPressed;
    
    bool _switchPressed;
    
    bool _deviceShaking;

#pragma mark Internal Touch Management   
	// The screen is divided into four zones: Left, Bottom, Right and Main/
	// These are all shown in the diagram below.
	//
	//   |---------------|
	//   |           |   |
	//   |---|   M   | R |
	//   | L |       |   |
	//   -----------------
	//
	// The meaning of any touch depends on the zone it begins in.

	/** Information representing a single "touch" (possibly multi-finger) */
	struct TouchInstance {
		/** The anchor touch position (on start) */
		cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
		/** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
	};

	/** Enumeration identifying a zone for the current touch */
	enum class Zone {
		/** The touch was not inside the screen bounds */
		UNDEFINED,
		/** The touch was in the left zone (as shown above) */
        JOY,
		/** The touch was in the right zone (as shown above) */
		RIGHT,
		/** The touch was in the main zone (as shown above) */
		MAIN
	};

	/** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
	cugl::Rect _sbounds;
	/** The bounds of the joy touch zone */
	cugl::Rect _jzone;
	/** The bounds of the right touch zone */
	cugl::Rect _rzone;

	// Each zone can have only one touch
	/** The current touch location for the joy zone */
	TouchInstance _jtouch;
	/** The current touch location for the right zone */
	TouchInstance _rtouch;
	/** The current touch location for the bottom zone */
	TouchInstance _mtouch;
    
    /** Whether the virtual joystick is active */
    bool _joystick;
    /** The position of the virtual joystick anchor */
    cugl::Vec2 _joyAnchor;
    /** The position of the virtual joystick dynamic */
    cugl::Vec2 _joyCenter;
    /** Whether or not we have processed a jump for this swipe yet */
    bool _hasJumped;
    /** The timestamp for a double tap on the right */
    cugl::Timestamp _rtime;
	/** The timestamp for a double tap in the middle */
	cugl::Timestamp _mtime;
    /** Maximum vector for joystick for scaling purposes */
//    cugl::Vec2 maxJoystickVec = Vec2()
    
    /** List holding points for swipe drawing */
    std::shared_ptr<std::list<std::pair<cugl::Vec2, cugl::Timestamp>>> _swipePoints;
    std::vector<cugl::Vec2> _swipePointsVec;
    cugl::Color4 _currentSwipeColor;
    std::deque<std::pair<cugl::Vec2, cugl::Timestamp>> _internalSwipePoints;
    
    
    
    std::shared_ptr<cugl::GestureRecognizer> _lineGesturer;
    std::shared_ptr<cugl::GestureRecognizer> _circleGesturer;


    /**
     * Defines the zone boundaries, so we can quickly categorize touches.
     */
	void createZones();
  
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);

    /**
     * Returns the correct zone for the given position.
     *
     * See the comments above for a description of how zones work.
     *
     * @param  pos  a position in screen coordinates
     *
     * @return the correct zone for the given position.
     */
    Zone getZone(const cugl::Vec2 pos) const;
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;

    /**
     * Processes movement for the floating joystick.
     *
     * This will register movement as left or right (or neither).  It
     * will also move the joystick anchor if the touch position moves
     * too far.
     *
     * @param  pos  the current joystick position
     */
    void processJoystick(const cugl::Vec2 pos);
    
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
	int processSwipe(const cugl::Vec2 start, const cugl::Vec2 stop, cugl::Timestamp current);
  
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
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
    bool init(const cugl::Rect bounds);

    static std::shared_ptr<InputController> alloc(const cugl::Rect bounds) {
        std::shared_ptr<InputController> result = std::make_shared<InputController>();
        return (result->init(bounds) ? result : nullptr);
    }
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

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
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
	cugl::Vec2 getMovement() const { return _movement; }

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
	bool didReset() const { return _resetPressed; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
	bool didDebug() const { return _debugPressed; }

	/**
	 * Returns true if the exit button was pressed.
	 *
	 * @return true if the exit button was pressed.
	 */
	bool didExit() const { return _exitPressed; }

    
    /**
     * Returns true if the virtual joystick is in use (touch only)
     *
     * @return true if the virtual joystick is in use (touch only)
     */
    bool withJoystick() const { return _joystick; }

    /**
     * Returns the scene graph position of the virtual joystick
     *
     * @return the scene graph position of the virtual joystick
     */
    std::pair<cugl::Vec2, cugl::Vec2> getJoystick() const { return std::pair(_joyAnchor, _joyCenter); }
    
    // CAN DELETE THIS
    bool didRustle() const { return _rustlePressed; }
    
    bool didShowPlayer() const { return _showPlayerPressed; }
    
    bool didDash() const { return _dashPressed; }
    
    bool didDashNow() const { return _keyDash; }
    
    bool didDashThisSwipe() const { return _dashPressedThisSwipe; }
    
    bool didRoot() const { return _rootPressed; }
    
    bool didRootNow() const { return _keyRoot; }
    
    bool didUnroot() const { return _unrootPressed; }
    
    bool didUnrootNow() const { return _keyUnroot; }

    bool didSwitch() const { return _switchPressed; }
    
    bool didShakeDevice() const { return _deviceShaking; }

#pragma mark -
#pragma mark Swipe Drawing Logic
    
    std::shared_ptr<std::list<std::pair<cugl::Vec2, cugl::Timestamp>>> getSwipePoints() {
        return _swipePoints;
    }
    
    std::vector<cugl::Vec2> getSwipePointsVector() {
        std::vector<cugl::Vec2> res;
        for (auto it = _swipePoints->begin(); it != _swipePoints->end(); it++) {
            res.push_back(it->first);
        }
        return res;
    }
    
    /**
     * Returns whether this point is 'notable'
     *
     * A notable point is one that
     * 1. is the first point added to an empty _internalSwipePoints_
     * 2. is greater than x^2 distance away from the previously added point in _internalSwipePoints_
     *
     * @param point the point to be identified as notable
     */
    bool isNotablePoint(cugl::Vec2 point) {
        if (_internalSwipePoints.empty()) {
            return true;
        }
        return _internalSwipePoints.front().first.distanceSquared(point) > SWIPE_DISTANCE_THRESHOLD;
    }
    
    void addSwipePoint(cugl::Vec2 point) {
        if (_swipePoints->size() == SWIPE_POINTS_CAPACITY) {
            _swipePoints->pop_back();
        }
        auto pointPair = std::pair(point, cugl::Timestamp());
        _swipePoints->push_front(pointPair);
        if (isNotablePoint(point)) {
            if (_internalSwipePoints.size() == INTERNAL_SWIPE_POINTS_CAPACITY) {
                _internalSwipePoints.pop_back();
            }
            _internalSwipePoints.push_front(pointPair);
        }
    }
    
    void clearSwipePoints() {
        _swipePoints->clear();
        _internalSwipePoints.clear();
    }
    
    void cullSwipePointsByDuration() {
        cugl::Timestamp curTime = cugl::Timestamp();
        for (auto it = _swipePoints->begin(); it != _swipePoints->end();) {
            if (it->second + SWIPE_DURATION_MILLIS < curTime) {
                it = _swipePoints->erase(it);
            }
            else {
                it++;
            }
        }
        for (auto it = _internalSwipePoints.begin(); it != _internalSwipePoints.end();) {
            if (it->second + INTERNAL_SWIPE_DURATION_MILLIS < curTime) {
                it = _internalSwipePoints.erase(it);
            }
            else {
                it++;
            }
        }
        
//        for (auto it = _swipePoints->rbegin(); it != _swipePoints->rend();) {
//            if (!((it->second + swipeDurationMillis) < curTime)) { // need a not here because > is not implemented with Timestamps for some reason
//                break;
//            }
//            std::advance(it, 1);
//            _swipePoints->erase(it.base());
//        }
//        for (auto it = _internalSwipePoints.rbegin(); it != _internalSwipePoints.rend();) {
//            if (!((it->second + swipeDurationMillis) < curTime)) { // need a not here because > is not implemented with Timestamps for some reason
//                break;
//            }
//            std::advance(it, 1);
//            _internalSwipePoints.erase(it.base());
//        }
    }
    
    cugl::Color4 getCurrentSwipeColor() {
        return _currentSwipeColor;
    }
    
    std::vector<cugl::Vec2> getInternalSwipePointsVector() {
        std::vector<cugl::Vec2> res;
        for (auto it = _internalSwipePoints.begin(); it != _internalSwipePoints.end(); it++) {
            res.push_back(it->first);
        }
        return res;
    }


#pragma mark -
#pragma mark Touch and Mouse Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus	Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

    int signum(int num);
};

#endif /* __PF_INPUT_H__ */
