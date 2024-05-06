//
//  HostScene.cpp
//  Rooted
//
//  Created by Joshua Guo on 3/6/24.
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "HostScene.h"
#include "../RootedConstants.h"

using namespace cugl;
using namespace cugl::net;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/**
 * Converts a hexadecimal string to a decimal string
 *
 * This function assumes that the string is 4 hexadecimal characters
 * or less, and therefore it converts to a decimal string of five
 * characters or less (as is the case with the lobby server). We
 * pad the decimal string with leading 0s to bring it to 5 characters
 * exactly.
 *
 * @param hex the hexadecimal string to convert
 *
 * @return the decimal equivalent to hex
 */
static std::string hex2dec(const std::string hex) {
    Uint32 value = strtool::stou32(hex,0,16);
    std::string result = strtool::to_string(value);
    if (result.size() < 5) {
        size_t diff = 5-result.size();
        std::string alt(5,'0');
        for(size_t ii = 0; ii < result.size(); ii++) {
            alt[diff+ii] = result[ii];
        }
        result = alt;
    }
    return result;
}

#pragma mark -
#pragma mark Provided Methods
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
bool HostScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    _network = network;
    
    // Start up the input handler
    _assets = assets;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("host");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD

    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host_lobbybuttons_host"));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("host_back"));
    _gameid = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host_lobbybuttons_idfield_textfield"));
    // currently no way to display the number of players
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("host_lobbybuttons_playercount_players"));
        
    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _network->disconnect();
            _backClicked = true;
        }
    });

    _startgame->addListener([this](const std::string& name, bool down) {
        if (!down) {
            startGame();
        }
    });
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    _sendCount = 0;
    _receiveCount = 0;
    _totalPing = 0;

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void HostScene::dispose() {
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
void HostScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        /**
         * TODO: if value is true, you need to activate the _backout button, and set the clicked variable to false. You need to also call the network controller to start a connection as a host. If the value is false, and reset all buttons and textfields to their original state.
         */
        if (value) {
            _backout->activate();
            _network->disconnect();
            _network->connectAsHost();
            updateText(_startgame, "start");
            _backClicked = false;
            _startGameClicked = false;
        } else {
            _gameid->setText("");
            _startgame->deactivate();
            updateText(_startgame, "INACTIVE");
            _backout->deactivate();
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backout->setDown(false);
        }
    }
}


/**
 * Updates the text in the given button.
 *
 * Techincally a button does not contain text. A button is simply a scene graph
 * node with one child for the up state and another for the down state. So to
 * change the text in one of our buttons, we have to descend the scene graph.
 * This method simplifies this process for you.
 *
 * @param button    The button to modify
 * @param text      The new text value
 */
void HostScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("play"));
    label->setText(text);
}

/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void HostScene::update(float timestep) {
    if(_network->getStatus() == NetEventController::Status::CONNECTED) {
        if (!_startGameClicked) {
            updateText(_startgame, "start");
            _startgame->activate();
        }
        else {
            updateText(_startgame, "...");
            _startgame->deactivate();
        }
        _gameid->setText(hex2dec(_network->getRoomID()));
        _player->setText(std::to_string(_network->getNumPlayers()));
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTING) {
        _gameid->setText("...");
    }
}

/**
 * This method prompts the network controller to start the game.
 */
void HostScene::startGame() {
    if (debug || _network->getNumPlayers() > 1) {
        _network->startGame();
        _startGameClicked = true;
    }
}
