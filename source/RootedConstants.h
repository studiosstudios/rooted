//
// Created by Jeffrey Zhou on 4/16/24.
//

#ifndef ROOTED_ROOTEDCONSTANTS_H
#define ROOTED_ROOTEDCONSTANTS_H

#pragma mark -
#pragma mark GAMESCENE

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024.0
#define SCENE_HEIGHT 576.0

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 9.0/16.0

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  18.0f
/** Default drawscale */
#define DEFAULT_DRAWSCALE 32.0f
/** Zoom of camera relative to scene */
#define DEFAULT_CAMERA_ZOOM 2.0f
/** Camera gliding rate */
#define CAMERA_GLIDE_RATE 0.06f

#pragma mark -
#pragma mark Physics Constants
/** No gravity because we are top down */
#define DEFAULT_GRAVITY 0.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The font for victory/failure messages */
#define MESSAGE_FONT    "gyparody"
/** The message for winning the game */
#define WIN_MESSAGE     "YOU WIN!"
/** The color of the win message */
#define WIN_COLOR       Color4::BLUE
/** The message for losing the game */
#define LOSE_MESSAGE    "YOU LOSE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The message for resetting the game */
#define RESET_MESSAGE    "RESETTING"
/** The color of the reset message */
#define RESET_COLOR      Color4::YELLOW
/** The key the victory game music */
#define WIN_MUSIC       "win"
/** The key the failure game music */
#define LOSE_MUSIC      "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT      "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT      "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT     "jump"
/** The volume for the music */
#define MUSIC_VOLUME    0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME   0.8f
/** The image for the left dpad/joystick */
#define LEFT_IMAGE      "dpad_left"
/** The image for the right dpad/joystick */
#define RIGHT_IMAGE     "dpad_right"

#define JOY_MAIN        "joystick-main"
#define JOY_BACK        "joystick-back"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::GREEN
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192

#pragma mark -
#pragma mark INPUT CONTROLLER

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** How close we need to be for a multi touch */
#define NEAR_TOUCH      100
/** The key for the event handlers */
#define LISTENER_KEY      1

/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  15
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    100
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    0
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400

#define SWITCH_SWIPE_LENGTH 125

// The screen is divided into three zones: Joy(stick), Main, and Right.
//
//   |---------------|
//   |   |       |   |
//   |---|   M   | R |
//   | J |       |   |
//   |---------------|
//
// The meaning of any touch depends on the zone it begins in.

/** The portion of the screen used for the left zone */
#define JOY_ZONE_WIDTH       0.35f
#define JOY_ZONE_HEIGHT      0.5f
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.35f

#pragma mark -
#pragma mark SHADERS
#define WIND_SPEED 1.0
const float WIND_DIRECTION[2] = {1.0, 1.0};
#define CLOUD_SPEED 0.05
#define DEFAULT_WHEAT_TEX_WIDTH 480
#define DEFAULT_WHEAT_TEX_HEIGHT 270
#define MAX_WHEAT_HEIGHT 15.0
#define STEP_SIZE 1.0



#endif //ROOTED_ROOTEDCONSTANTS_H
