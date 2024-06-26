//
//  MenuScene.cpp
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "MenuScene.h"

using namespace cugl;
using namespace std;

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
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
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    _menuscene = _assets->get<scene2::SceneNode>("mainmenu");
    _menuscene->setContentSize(dimen);
    _menuscene->doLayout(); // Repositions the HUD
    
    _lobbyscene = _assets->get<scene2::SceneNode>("prelobby");
    _lobbyscene->setContentSize(dimen);
    _lobbyscene->doLayout();
    
    _choice = Choice::NONE;
    _currmenuchoice = Choice::MAIN;
        
    // add all button behaviors
    _playbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_play"));
    _optionsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_options"));
    _statsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_stats"));
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_buttons_host"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_buttons_join"));
    
    _backoutprelobby = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_back"));
    
    // Program the buttons
    _playbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::LOBBY;
            _currmenuchoice = Choice::LOBBY;
        }
    });
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::HOST;
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::JOIN;
        }
    });
    _backoutprelobby->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::MAIN;
        }
    });

    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            switchScene(_currmenuchoice);
        } else {
            // deactivate all buttons
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            _playbutton->deactivate();
            _statsbutton->deactivate();
            _optionsbutton->deactivate();
            _backoutprelobby->deactivate();
            // If any were pressed, reset them
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
            _playbutton->setDown(false);
            _statsbutton->setDown(false);
            _optionsbutton->setDown(false);
            _backoutprelobby->setDown(false);
        }
    }
}

void MenuScene::update(float timestep) {
    if (_choice != Choice::HOST && _choice != Choice::JOIN) {
        switchScene(_choice);
        _choice = NONE;
    }
}

void MenuScene::switchScene(MenuScene::Choice sceneType) {
    // TODO: this should be more automated
    if (isActive()) {
        switch (sceneType) {
            case MAIN:
                removeAllChildren();
                addChild(_menuscene);
                _hostbutton->deactivate();
                _joinbutton->deactivate();
                _backoutprelobby->deactivate();
                _playbutton->activate();
                _statsbutton->activate();
                _optionsbutton->activate();
                break;
            case LOBBY:
                removeAllChildren();
                addChild(_lobbyscene);
                _hostbutton->activate();
                _joinbutton->activate();
                _backoutprelobby->activate();
                _playbutton->deactivate();
                _statsbutton->deactivate();
                _optionsbutton->deactivate();
                break;
            default:
                break;
        }
    }
}
