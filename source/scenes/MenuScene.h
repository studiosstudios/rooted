//
//  MenuScene.h
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//

#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include "../RootedConstants.h"

class MenuScene : public cugl::Scene2 {
public:
    /**
     * The menu choice.
     *
     * This state allows the top level application to know what the user
     * chose.
     */
    enum Choice {
        /** User has not yet made a choice */
        NONE,
        /** User wants to host a game */
        HOST,
        /** User wants to join a game */
        JOIN,
        /** User wants to play the tutorial */
        TUTORIAL,

        /** These are scenes within this scene */
        MAIN,
        LOBBY,
        STATS,
        SETTINGS
    };
    
    bool hapticsTrue();
    bool swipeTrue();
    float musicSliderVal();
    float soundSliderVal();

protected:
    /** The asset manager for this scene. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** Main menu screen */
    std::shared_ptr<cugl::scene2::SceneNode> _menuscene;
    /** Lobby screen */
    std::shared_ptr<cugl::scene2::SceneNode> _lobbyscene;
    /** Stats screen */
    std::shared_ptr<cugl::scene2::SceneNode> _statsscene;
    /** Options scene **/
    std::shared_ptr<cugl::scene2::SceneNode> _optionsscene;
    
    // TODO put scenes and their buttons together
    /** The menu button for hosting a game */
    std::shared_ptr<cugl::scene2::Button> _hostbutton;
    /** The menu button for joining a game */
    std::shared_ptr<cugl::scene2::Button> _joinbutton;
    /** The menu button for choosing to bring to lobby selection screen */
    std::shared_ptr<cugl::scene2::Button> _playbutton;
    /** The menu button to see your own stats */
    std::shared_ptr<cugl::scene2::Button> _statsbutton;
    /** The menu button to change options */
    std::shared_ptr<cugl::scene2::Button> _optionsbutton;
    /** The menu button to back out into title screen from the prelobby */
    std::shared_ptr<cugl::scene2::Button> _backoutprelobby;
    
    std::shared_ptr<cugl::scene2::Button> _backoutoptions;
    std::shared_ptr<cugl::scene2::Button> _joystickdirbutton;
    std::shared_ptr<cugl::scene2::Button> _swipedirbutton;
    std::shared_ptr<cugl::scene2::Button> _dashinfobutton;
    std::shared_ptr<cugl::scene2::Button> _hapticsbutton;
    std::shared_ptr<cugl::scene2::Slider> _soundslider;
    std::shared_ptr<cugl::scene2::Slider> _musicslider;
    std::shared_ptr<cugl::scene2::SceneNode> _popup;
    std::shared_ptr<cugl::scene2::Button> _popupclose;
    
    /** The player menu choice (for the external scene manager, i.e. the main app) */
    Choice _choice;
    /** The player choice when changing menus within this scene */
    Choice _currmenuchoice;
    /** The map connecting the current "scene" with the Buttons associated with that "scene" (excluding the actual scene) */
    std::map<Choice, std::vector<std::shared_ptr<cugl::scene2::Button>>> _screenButtonMap;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new  menu scene with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    MenuScene() : cugl::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~MenuScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents.
     *
     * In previous labs, this method "started" the scene.  But in this
     * case, we only use to initialize the scene user interface.  We
     * do not activate the user interface yet, as an active user
     * interface will still receive input EVEN WHEN IT IS HIDDEN.
     *
     * That is why we have the method {@link #setActive}.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Sets whether the scene is currently active
     *
     * This method should be used to toggle all the UI elements.  Buttons
     * should be activated when it is made active and deactivated when
     * it is not.
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
    
    void update(float timestep) override;
    
    /**
     * Returns the user's menu choice.
     *
     * This will return NONE if the user had no yet made a choice.
     *
     * @return the user's menu choice.
     */
    Choice getChoice() const { return _choice; }
    
    void switchScene(MenuScene::Choice sceneType);
    
    void togglePopup(bool active);
    
};

#endif /* __MENU_SCENE_H__ */
