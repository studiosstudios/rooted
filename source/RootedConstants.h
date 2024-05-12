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
/** Width of the game world in Box2d units */
#define MAP_UNIT_WIDTH   16.0f
/** Height of the game world in Box2d units */
#define MAP_UNIT_HEIGHT   9.0f
/** Default drawscale */
#define DEFAULT_DRAWSCALE 32.0f
/** Zoom of camera relative to scene */
#define DEFAULT_CAMERA_ZOOM 2.0f
/** Camera gliding rate */
#define CAMERA_GLIDE_RATE 0.06f
/** Camera zoom rate **/
#define ZOOM_RATE 0.02f
/** Camera zoom error **/
#define ZOOM_ERR 0.005f
/** Camera position error **/
#define CAM_POSITION_ERR 0.01f
/** Number of frames to hold zoom out */
#define HOLD_CAM 40

#pragma mark -
#pragma mark Physics Constants
/** No gravity because we are top down */
#define DEFAULT_GRAVITY 0.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240

#pragma mark -
#pragma mark Game Logic Constants
/** Time in seconds that must elapse before being able to perform another dash */
#define DASH_COOLDOWN_SECS 3
/** Time in seconds that being hit by a rock stuns a player */
#define STUN_SECS 1
/** Max camera coordinate units that the camera can shake when player is stunned */
#define STUN_SCREEN_SHAKE 5

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
#define JSTICK_RADIUS    256
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    0
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400

// SWIPE GESTURE CONSTANTS
/** How far a swipe must go from its first "notable point" to its most recent "notable point" to be considered a swipe */
#define SWIPE_LENGTH    150
/** The maximum number of swipe points that are stored at once (for drawing purposes) */
#define SWIPE_POINTS_CAPACITY 25
/** The maximum number of internal swipe points that are stored at once (for gesture recognition purposes) */
#define INTERNAL_SWIPE_POINTS_CAPACITY 15
/** The number of "notable points" a swipe must contain to be compared for similarity.*/
#define SWIPE_POINT_MINIMUM 4
/** The minimum similarity score a dash line swipe gesture must make in order to be considered a dash */
#define LINE_GESTURE_SIMILARITY 0.96
/** The minimum similarity score a circle swipe gesture must make in order to be considered a root/unroot (forgiving score because circle can have more variance)*/
#define CIRCLE_GESTURE_SIMILARITY 0.7
/** The minimum distance a swipe move point must be from the last notable point to be considered a "notable point". This is a squared value, so 41^2 */
#define SWIPE_DISTANCE_THRESHOLD 1681
/** The length of time (in milliseconds) that a swipe point remains stored */
#define SWIPE_DURATION_MILLIS 225
/** The length of time (in milliseconds) that an internal swipe point remains stored*/
#define INTERNAL_SWIPE_DURATION_MILLIS 225

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
#define MAX_WHEAT_HEIGHT 25.0
#define STEP_SIZE 1.0


#pragma mark COLLECTIBLE
#define SPAWN_RATE 0.005
#define MAX_NUM_COLLECTIBLES 8
#define MAX_COLLECTIBLE_AGE 3
#define SPAWN_COOLDOWN 600
#define THROW_SPEED 10
#define MIN_STUN_IMPULSE 3.0

#define PROGRESS 0.02f

#endif //ROOTED_ROOTEDCONSTANTS_H
