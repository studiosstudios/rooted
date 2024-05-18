//
//  NLClientScene.cpp
//  Network Lab
//
//  This class represents the scene for the client when joining a game. Normally
//  this class would be combined with the class for the client scene (as both
//  initialize the network controller).  But we have separated to make the code
//  a little clearer for this lab.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "ClientScene.h"

using namespace cugl;
using namespace cugl::physics2::net;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720

/**
 * Converts a decimal string to a hexadecimal string
 *
 * This function assumes that the string is a decimal number less
 * than 65535, and therefore converts to a hexadecimal string of four
 * characters or less (as is the case with the lobby server). We
 * pad the hexadecimal string with leading 0s to bring it to four
 * characters exactly.
 *
 * @param dec the decimal string to convert
 *
 * @return the hexadecimal equivalent to dec
 */
static std::string dec2hex(const std::string dec) {
    Uint32 value = strtool::stou32(dec);
    if (value >= 655366) {
        value = 0;
    }
    return strtool::to_hexstring(value,4);
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool ClientScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<NetworkController> network) {
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
    _network = network;
    
    // Acquire the scene built by the asset loader and resize it the scene
    _clientscene = _assets->get<scene2::SceneNode>("client");
    _clientscene->setContentSize(dimen);
    _clientscene->doLayout(); // Repositions the HUD
    
    // FOR JOINING THE GAME
    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client_join"));
    _backoutclient = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client_back"));
    _gameid = std::dynamic_pointer_cast<scene2::TextField>(_assets->get<scene2::SceneNode>("client_lobbyinfo_textbox_textfield"));
    
    std::string numbers[10]= { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
    for (std::string number : numbers) {
        _numbers.push_back(std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client_numpad_" + number)));
    }
    
    _backspace = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("client_lobbyinfo_textbox_backspace"));
    
    for (int ii = 0; ii < _numbers.size(); ii++) {
        auto n = _numbers.at(ii);
        n->addListener([this, ii](const std::string& name, bool down) {
            if(down){
                std::shared_ptr<Sound> source = _assets->get<Sound>(C_BUTTON_EFFECT);
                AudioEngine::get()->play("carrotButton", source, false, _soundScale);
            }
            if (!down && _gameid->getText().length() < 5) {
                _gameid->setText(_gameid->getText() + std::to_string(ii));
            }
        });
    }

    _backspace->addListener([this](const std::string& name, bool down) {
        if (!down && !_gameid->getText().empty()) {
            _gameid->setText(_gameid->getText().substr(0, _gameid->getText().length() - 1));
        }
    });
    
    _backoutclient->addListener([this](const std::string& name, bool down) {
        if(down){
            std::shared_ptr<Sound> source = _assets->get<Sound>(BUTTON_EFFECT);
            AudioEngine::get()->play("button", source, false, _soundScale);
        }
        else {
            _backClicked = true;
            _network->disconnect();
        }
    });

    _startgame->addListener([=](const std::string& name, bool down) {
        if(down){
            std::shared_ptr<Sound> source = _assets->get<Sound>(BUTTON_EFFECT);
            AudioEngine::get()->play("button", source, false, _soundScale);
        }
        else{
            if (!_gameid->getText().empty()) {
//                switchScene();
                _network->connectAsClient(dec2hex(_gameid->getText()));
            }
        }
    });

    // FOR THE LOADING/WAITING SCREEN
    // TODO: REMOVE THIS DEPENDING ON IF WE STILL HAVE THIS
    _loadingscene = _assets->get<scene2::SceneNode>("loadingscreen");
    _loadingscene->setContentSize(dimen);
    _loadingscene->doLayout();
    
    _backoutloading = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("loadingscreen_back"));
    // TODO: THIS IS CURRENTLY IN THE MAIN SCENE
    _player = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("client_lobbyinfo_textbox_2_textfield"));
    _loadingtext = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("loadingscreen_loadingtxt"));
    
    _backoutloading->addListener([this](const std::string& name, bool down) {
        if (!down) {
            switchScene();
            // clear text
            _gameid->setText("");
            _network->disconnect();
        }
    });
    
    // Create the server configuration
    auto json = _assets->get<JsonValue>("server");
    _config.set(json);
    
    addChild(_clientscene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void ClientScene::dispose() {
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
void ClientScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        
        if (value) {
            removeAllChildren();
            addChild(_clientscene);
            // only want to activate if you are not a touchscreen
            #ifndef CU_TOUCH_SCREEN
            _gameid->activate();
            #endif
            _gameid->setText("");
            _backoutclient->activate();
            configureStartButton();
            _backClicked = false;
            // Don't reset the room id
            
            // activate all of the numbers
            for (auto n : _numbers) {
                n->activate();
            }
            _backspace->activate();
        } else {
            _gameid->deactivate();
            _startgame->deactivate();
            _backoutclient->deactivate();
            _backoutloading->deactivate();
            //_network = nullptr;
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backoutclient->setDown(false);
            _backoutloading->setDown(false);

            for (auto n : _numbers) {
                n->deactivate();
                n->setDown(false);
            }
            _backspace->deactivate();
            _backspace->setDown(false);
        }
    }
}

/**
 * Checks that the network connection is still active.
 *
 * Even if you are not sending messages all that often, you need to be calling
 * this method regularly. This method is used to determine the current state
 * of the scene.
 *
 * @return true if the network connection is still active.
 */
void ClientScene::updateText(const std::shared_ptr<scene2::Button>& button, const std::string text) {
    auto label = std::dynamic_pointer_cast<scene2::Label>(button->getChildByName("stats"));
    label->setText(text);
}

/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ClientScene::update(float timestep) {
    // Do this last for button safety
    configureStartButton();
    if(_network->getStatus() == NetEventController::Status::CONNECTED || _network->getStatus() == NetEventController::Status::HANDSHAKE){
        _loadingtext->setText("WAITING");
        int numPlayers = _network->getNumPlayers();
        if (numPlayers == 1) {
            _player->setText(std::to_string(_network->getNumPlayers()) + " player");
        }
        else {
            _player->setText(std::to_string(_network->getNumPlayers()) + " players");
        }
    }
    else if (_network->getStatus() == NetEventController::Status::NETERROR) {
        _loadingtext->setText("ERROR :(");
    }
    else {
        _loadingtext->setText("LOADING...");
        _player->setText("...");
    }
}

/**
 * Reconfigures the start button for this scene
 *
 * This is necessary because what the buttons do depends on the state of the
 * networking.
 */
void ClientScene::configureStartButton() {
    if (_network->getStatus() == NetEventController::Status::IDLE) {
        _startgame->setDown(false);
        _startgame->activate();
        updateText(_startgame, "join");
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTING) {
        _startgame->setDown(false);
        _startgame->deactivate();
        updateText(_startgame, "...");
    }
    else if (_network->getStatus() == NetEventController::Status::CONNECTED) {
        _startgame->setDown(false);
        _startgame->deactivate();
        updateText(_startgame, "wait...");
    }
}

void ClientScene::switchScene() {
    if (isActive()) {
        if (getChildByName("client")) {
            removeAllChildren();
            addChild(_loadingscene);
            _backoutloading->activate();
            _backoutclient->deactivate();
            _backoutclient->setDown(false);
            _startgame->setDown(false);
            _startgame->deactivate();
        }
        else if (getChildByName("loadingscreen")) {
            removeAllChildren();
            addChild(_clientscene);
            _backoutclient->activate();
            _startgame->activate();
            // for some reason deactivating this breaks the game
//            _backoutloading->deactivate();
//            _backoutloading->setDown(false);
        }
    }
}
