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
    
    // SET UP MAIN MENU SCENE
    _menuscene = _assets->get<scene2::SceneNode>("mainmenu");
    _menuscene->setContentSize(dimen);
    _menuscene->doLayout(); // Repositions the HUD
    
    _playbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_play"));
    _optionsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_options"));
    _statsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mainmenu_menu_menubuttons_stats"));
    
    std::vector<std::shared_ptr<scene2::Button>> menubuttons = {_playbutton, _optionsbutton, _statsbutton};
    
    _screenButtonMap.insert({Choice::MAIN, menubuttons});
    
    // Program the buttons
    _playbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::LOBBY;
            _currmenuchoice = Choice::LOBBY;
        }
    });
    
    // SET UP LOBBY SCENE
    _lobbyscene = _assets->get<scene2::SceneNode>("prelobby");
    _lobbyscene->setContentSize(dimen);
    _lobbyscene->doLayout();
    
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_buttons_host"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_buttons_join"));
    _backoutprelobby = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("prelobby_back"));
    
    std::vector<std::shared_ptr<scene2::Button>> lobbybuttons = {_hostbutton, _joinbutton, _backoutprelobby};
    
    _screenButtonMap.insert({Choice::LOBBY, lobbybuttons});
    
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
            _currmenuchoice = Choice::MAIN;
        }
    });
    
    // SET UP OPTIONS SCENE
    _optionsscene = _assets->get<scene2::SceneNode>("optionsmenu");
    _optionsscene->setContentSize(dimen);
    _optionsscene->doLayout();
    
    _backoutoptions = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_back"));
    _backoutoptions->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::MAIN;
            _currmenuchoice = Choice::MAIN;
        }
    });
    _joystickdirbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_dashoption_joystick_joystickdirection"));
    _joystickdirbutton->setToggle(true);
    _joystickdirbutton->setDown(false);
    _joystickdirbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _swipedirbutton->setDown(false);
        } else {
            _swipedirbutton->setDown(true);
        }
    });
    _swipedirbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_dashoption_swipe_swipedirection"));
    _swipedirbutton->setToggle(true);
    _swipedirbutton->setDown(true);
    _swipedirbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _joystickdirbutton->setDown(false);
        } else {
            _joystickdirbutton->setDown(true);
        }
    });
    _dashinfobutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_dashoption_dashinfo"));
    _dashinfobutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            togglePopup(true);
        }
    });
    _hapticsbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_hapticsoption_boxselect"));
    _hapticsbutton->setToggle(true);
    _hapticsbutton->setDown(true);
    _hapticsbutton->addListener([this](const std::string& name, bool down) {
        CULog("toggle button");
    });
    _soundslider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_soundoption_slider"));
    _soundslider->addListener([this](const std::string& name, bool down) {
        if (!down) {
            CULog("sound");
        }
    });
    _musicslider = std::dynamic_pointer_cast<scene2::Slider>(_assets->get<scene2::SceneNode>("optionsmenu_optionsinfo_optionselection_musicoption_slider"));
    _musicslider->addListener([this](const std::string& name, bool down) {
        if (!down) {
            CULog("music");
        }
    });
    // do not add these to the map
    _popup = _assets->get<scene2::SceneNode>("optionsmenu_popup");
    _popupclose = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("optionsmenu_popup_exitpopup"));
    _popupclose->addListener([this](const std::string& name, bool down) {
        if (!down) {
            togglePopup(false);
        }
    });
    
#ifndef CU_TOUCH_SCREEN
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
#endif
    
    std::vector<std::shared_ptr<scene2::Button>> optionsbuttons = {_backoutoptions, _dashinfobutton};
    _screenButtonMap.insert({Choice::SETTINGS, optionsbuttons});
    
    _optionsbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::SETTINGS;
        }
    });
    
    // TODO: SET UP STATS SCENE HERE
    // only temp until we have another way to access the tutorial again
    _statsbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _choice = Choice::TUTORIAL;
        }
    });
    
    // set choices
    _choice = NONE;
    _currmenuchoice = MAIN;

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
            for (const auto& pair : _screenButtonMap) {
                for (const auto& b : pair.second) {
                    b->deactivate();
                    b->setDown(false);
                }
            }
        }
    }
}

void MenuScene::update(float timestep) {
    // check if a real scene switching choice
    if (_choice != HOST && _choice != JOIN && _choice != TUTORIAL &&  _choice != NONE) {
        switchScene(_choice);
        _choice = NONE;
    }
}

void MenuScene::switchScene(MenuScene::Choice sceneType) {
    if (isActive()) {
        removeAllChildren();
        // deactivate all buttons
        for (const auto& pair : _screenButtonMap) {
            for (const auto& b : pair.second) {
                b->deactivate();
            }
        }
        // activate the switched scenes buttons
        for (const auto& b : _screenButtonMap[sceneType]) {
            b->activate();
        }
        switch (sceneType) {
            case MAIN:
                _swipedirbutton->deactivate();
                _joystickdirbutton->deactivate();
                _soundslider->deactivate();
                _musicslider->deactivate();
                _hapticsbutton->deactivate();
                addChild(_menuscene);
                break;
            case LOBBY:
                _swipedirbutton->deactivate();
                _joystickdirbutton->deactivate();
                _soundslider->deactivate();
                _musicslider->deactivate();
                _hapticsbutton->deactivate();
                addChild(_lobbyscene);
                break;
            case SETTINGS:
                // activate stuff for settings specific things
                _swipedirbutton->activate();
                _joystickdirbutton->activate();
                _soundslider->activate();
                _musicslider->activate();
                _hapticsbutton->activate();
                addChild(_optionsscene);
                break;
            default:
                break;
        }
    }
}

void MenuScene::togglePopup(bool active) {
    _popup->setVisible(active);
    if (active) {
        _popupclose->activate();
    }
}

bool MenuScene::hapticsTrue(){
    return _hapticsbutton->isDown();
}

bool MenuScene::swipeTrue(){
    return _swipedirbutton->isDown();
}

float MenuScene::musicSliderVal(){
    return _musicslider->getValue();
}

float MenuScene::soundSliderVal(){
    return _soundslider->getValue();
}
