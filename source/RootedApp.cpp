//
//  Created by Kimmy Lin on 2/23/24.
//

#include "RootedApp.h"

using namespace cugl;

#pragma mark -
#pragma mark Application State
/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void RootedApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    Input::activate<Keyboard>();
    Input::activate<TextInput>();
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<Map>(GenericLoader<Map>::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets);
    _status = LOAD;
    
    // Que up the other assets
    AudioEngine::start();
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    _assets->loadAsync<Map>("map", "json/map.json", nullptr);
    
    cugl::net::NetworkLayer::start(net::NetworkLayer::Log::INFO);
    
    Application::onStartup(); // YOU MUST END with call to parent
    setDeterministic(true);
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void RootedApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _mainmenu.dispose();
    _hostgame.dispose();
    _joingame.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    
    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void RootedApp::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void RootedApp::onResume() {
    AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark Application Loop

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void RootedApp::update(float dt) {
//    if (!_loaded && _loading.isActive()) {
//        _loading.update(0.01f);
//    } else if (!_loaded) {
//        _loading.dispose(); // Disables the input listeners in this mode
//        _gameplay.init(_assets);
//        _loaded = true;
//        
//        // Switch to deterministic mode
//        setDeterministic(true);
//    }
}

/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void RootedApp::preUpdate(float dt) {
    if (!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (_status == LOAD) {
        // I don't think this is how I should do it but if it works for now it works.
        _network = NetworkController::alloc(NetEventController::alloc(_assets));
        _loading.dispose(); // Disables the input listeners in this mode
        _mainmenu.init(_assets);
        _mainmenu.setActive(true);
        _hostgame.init(_assets,_network);
        _joingame.init(_assets,_network);
        _loaded = true;
        _status = MENU;
    } else if (_status == MENU) {
        updateMenuScene(dt);
    }
    else if (_status == HOST){
        updateHostScene(dt);
    }
    else if (_status == CLIENT){
        updateClientScene(dt);
    }
    else if (_status == GAME){
        if(_gameplay.isComplete()){
            _gameplay.reset();
            _status = MENU;
            _mainmenu.setActive(true);
        }
        _gameplay.preUpdate(dt);
    }
}

/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 */
void RootedApp::fixedUpdate() {
    // Compute time to report to game scene version of fixedUpdate
    float time = getFixedStep()/1000000.0f;
//    _gameplay.fixedUpdate(time);
    if (_status == GAME) {
        _gameplay.fixedUpdate(time);
    }
    if(_network){
        _network->_network->updateNet();
    }
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #preUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void RootedApp::postUpdate(float dt) {
    // Compute time to report to game scene version of postUpdate
    float time = getFixedRemainder()/1000000.0f;
//    _gameplay.postUpdate(time);
    if (_status == GAME) {
        _gameplay.postUpdate(time);
    }
}

/**
 * Inidividualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void RootedApp::updateMenuScene(float timestep) {
    _mainmenu.update(timestep);
    switch (_mainmenu.getChoice()) {
        case MenuScene::Choice::HOST:
            _mainmenu.setActive(false);
            _hostgame.setActive(true);
            _status = HOST;
            break;
        case MenuScene::Choice::JOIN:
            _mainmenu.setActive(false);
            _joingame.setActive(true);
            _status = CLIENT;
            break;
        case MenuScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void RootedApp::updateHostScene(float timestep) {
    _hostgame.update(timestep);
    if(_hostgame.getBackClicked()){
        _status = MENU;
        _hostgame.setActive(false);
        _mainmenu.setActive(true);
    }
    else if (_network->_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->_network->getShortUID()) {
        _gameplay.init(_assets, _network, true);
        _network->_network->markReady();
    }
    else if (_network->_network->getStatus() == NetEventController::Status::INGAME) {
        _hostgame.setActive(false);
        _gameplay.setActive(true);
        _status = GAME;
    }
    else if (_network->_network->getStatus() == NetEventController::Status::NETERROR) {
        _network->_network->disconnect();
        _hostgame.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.dispose();
        _status = MENU;
    }
}

/**
 * Inidividualized update method for the client scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void RootedApp::updateClientScene(float timestep) {
    _joingame.update(timestep);
    if(_joingame.getBackClicked()){
        _status = MENU;
        _joingame.setActive(false);
        _mainmenu.setActive(true);
    }
    else if (_network->_network->getStatus() == NetEventController::Status::HANDSHAKE && _network->_network->getShortUID()) {
        _gameplay.init(_assets, _network, false);
        _network->_network->markReady();
    }
    else if (_network->_network->getStatus() == NetEventController::Status::INGAME) {
        _joingame.setActive(false);
        _gameplay.setActive(true);
        _status = GAME;
    }
    else if (_network->_network->getStatus() == NetEventController::Status::NETERROR) {
        _network->_network->disconnect();
        _joingame.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.dispose();
        _status = MENU;
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void RootedApp::draw() {
    switch (_status) {
        case LOAD:
            _loading.render(_batch);
            break;
        case MENU:
            _mainmenu.render(_batch);
            break;
        case HOST:
            _hostgame.render(_batch);
            break;
        case CLIENT:
            _joingame.render(_batch);
            break;
        case GAME:
            _gameplay.render(_batch);
        default:
            break;
    }
}
