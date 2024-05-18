//
//  UIController.cpp
//  Rooted
//
//  Created by Choong Jae Lee on 2/27/24.
//

#include "UIController.h"
#include "../RootedConstants.h"

using namespace cugl;

void UIController::dispose() {
    if (_uinode != nullptr){
        _uinode->removeAllChildren();
    }
    _assets = nullptr;
    _input = nullptr;
    _uinode = nullptr;
}

void UIController::setWinVisible(bool visible) {
    _winNode->setVisible(visible);
}

void UIController::setLoseVisible(bool visible) {
    _loseNode->setVisible(visible);
}

void UIController::setEndVisible(bool visible) {
    _postroundscene->setVisible(visible);
    if (visible) {
        _nextbutton->activate();
    } else {
        _nextbutton->deactivate();
        _nextbutton->setDown(false);
    }
}

void UIController::setEndVariables(int roundNum, int length, int babies, int carrots) {
    _roundNumber->setText("ROUND " + std::to_string(roundNum));
    int seconds = length / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    _time->setText(std::to_string(minutes) + ":" + std::to_string(seconds));
    _babyCarrots->setText(std::to_string(babies));
    for (int ii = 0; ii < _carrotsRooted.size(); ii++) {
        _carrotsRooted.at(ii)->setVisible(ii < carrots);
    }
}

void UIController::setSpeechBubbleVisible(bool visible) {
    _speechBubble->setVisible(visible);
}

void UIController::setDialogBoxVisible(bool visible) {
    _easingtime = 0;
    _dialogBoxVisible = visible;
}

void UIController::setDialogBoxText(std::string text) {
    _dialogBoxText->setText(text);
}

void UIController::initJoystickNodes() {
    _joynode = scene2::SceneNode::allocWithBounds(Vec2(JOY_ZONE_WIDTH * SCENE_WIDTH / _cameraZoom, JOY_ZONE_HEIGHT * SCENE_HEIGHT / _cameraZoom) + _offset);

    _joyback = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("joystick-back"));
    _joyback->setScale(0.5f / _cameraZoom);
    _joynode->addChild(_joyback);
    
    _joymain = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("joystick"));
    _joymain->setScale(0.25f / _cameraZoom);
    _joymain->setVisible(false);
    _joynode->addChild(_joymain);
    
    _uinode->addChild(_joynode);
}

void UIController::initGameUINodes() {
    _winNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("victory"));
    _winNode->setPosition((Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2) - _offset) / _cameraZoom);
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setScale(_drawScale/DEFAULT_DRAWSCALE);
    _winNode->setVisible(false);
    _uinode->addChild(_winNode);
    
    _loseNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("defeat"));
    _loseNode->setPosition((Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2) - _offset)/ _cameraZoom);
    _loseNode->setScale(_drawScale/DEFAULT_DRAWSCALE);
    _loseNode->setAnchor(Vec2::ANCHOR_CENTER);
    _loseNode->setVisible(false);
    _uinode->addChild(_loseNode);
    
    _carrotsRemainingBoard = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("carrot-count-board"));
    _carrotsRemainingBoard->setPosition((Vec2(_carrotsRemainingBoard->getContentWidth(), SCENE_HEIGHT - _carrotsRemainingBoard->getContentHeight()) - _offset) / _cameraZoom);
    _uinode->addChild(_carrotsRemainingBoard);
    
    _carrotsRemainingText = scene2::Label::allocWithText("remaining player carrots: 2", _assets->get<Font>("gaeguBold16"));
    _carrotsRemainingText->setHorizontalAlignment(HorizontalAlign::CENTER);
    _carrotsRemainingText->setVerticalAlignment(VerticalAlign::MIDDLE);
    _carrotsRemainingText->setContentSize(_carrotsRemainingBoard->getContentSize());
    _carrotsRemainingText->doLayout();
    _carrotsRemainingBoard->addChild(_carrotsRemainingText);
    
    _barrotsRemainingBoard = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("barrot-count-board"));
    _barrotsRemainingBoard->setPosition((Vec2(SCENE_WIDTH - _barrotsRemainingBoard->getContentWidth(), SCENE_HEIGHT - _barrotsRemainingBoard->getContentHeight()) + _offset) / _cameraZoom);
    _uinode->addChild(_barrotsRemainingBoard);
    
    _barrotsRemainingText = scene2::Label::allocWithText("remaining baby carrots: 2", _assets->get<Font>("gaeguBold16"));
    _barrotsRemainingText->setHorizontalAlignment(HorizontalAlign::CENTER);
    _barrotsRemainingText->setVerticalAlignment(VerticalAlign::MIDDLE);
    _barrotsRemainingText->setContentSize(_barrotsRemainingBoard->getContentSize());
    _barrotsRemainingText->doLayout();
    _barrotsRemainingBoard->addChild(_barrotsRemainingText);

    // TUTORIAL UI STUFF
    _dialogBox = scene2::NinePatch::allocWithTexture(_assets->get<Texture>("dialoguebox"), Rect(137, 137, 512, 512));
    _dialogBox->SceneNode::setContentSize(Size(2000, 500));
    _dialogBox->setPosition((Vec2(SCENE_WIDTH/2, SCENE_HEIGHT * 0.25)) / _cameraZoom);
    _dialogBox->setAnchor(Vec2::ANCHOR_CENTER);
    _dialogBox->setScale(0);
    _dialogBox->setColor(Color4(255,255,255,255));
    _dialogBox->setVisible(true);
    _uinode->addChild(_dialogBox);
    _dialogBoxVisible = false;
    _dialogBoxFullScale = 0.2 * _drawScale/DEFAULT_DRAWSCALE;
    _easingtime = 1.0;
    
    _dialogBoxText = scene2::Label::allocWithText("Your baby carrots have escaped!\nUse the joystick on left side of screen to move.", _assets->get<Font>("gaeguBold75"));
    _dialogBoxText->setHorizontalAlignment(HorizontalAlign::CENTER);
    _dialogBoxText->setVerticalAlignment(VerticalAlign::MIDDLE);
    _dialogBoxText->setPadding(150, 0, 150, 0);
    _dialogBoxText->setContentSize(_dialogBox->getContentSize());
    _dialogBoxText->setRelativeColor(false);
    _dialogBoxText->doLayout();
    _dialogBoxText->setWrap(true);
    _dialogBox->addChild(_dialogBoxText);

    _dashTimerNode = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2::ZERO, 32));
    _dashTimerNode->setPosition(((Vec2(SCENE_WIDTH* 1.25f, SCENE_HEIGHT * 0.1f)) - _offset ) / _cameraZoom);
    _dashTimerNode->setScale(_drawScale/DEFAULT_DRAWSCALE);
    _dashTimerNode->setColor(Color4::GREEN);
    _uinode->addChild(_dashTimerNode);

}

bool UIController::init(const std::shared_ptr<cugl::AssetManager>& assets,
                        const std::shared_ptr<InputController>& input,
                        const std::shared_ptr<cugl::scene2::SceneNode> uinode,
                        Vec2 offset, float cameraZoom, float drawScale) {
    _assets = assets;
    _input = input;
    _uinode = uinode;
    _offset = offset;
    _cameraZoom = cameraZoom;
    _drawScale = drawScale;
    
    _uinode->setContentSize(SCENE_WIDTH / _cameraZoom, SCENE_HEIGHT / _cameraZoom);
    
    initGameUINodes();
    initJoystickNodes();
    
    _swipeNode = scene2::PolygonNode::alloc();
    _swipeNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _uinode->addChild(_swipeNode);
    
    // post round scene info
    _postroundscene = _assets->get<scene2::SceneNode>("postround");
    _postroundscene->setScale(1 / _cameraZoom);
    _postroundscene->doLayout(); // Repositions the HUD
    _postroundscene->setVisible(false);
    _uinode->addChild(_postroundscene);
    
    _roundNumber = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_roundnum"));
    _time = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_gamelength_gametime"));
    _babyCarrots = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_babycarrotscount_numbabycarrots"));
    _carrotsRooted = {
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carrot"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carrot_1"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carrot_2"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carrot_3"),
    };
    
    _nextbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("postround_next"));
    _nextbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _postroundscene->setVisible(false);
            _playerpointinfo->setVisible(true);
            _nextroundbutton->activate();
            _exitbutton->activate();
        }
    });
    
    _playerpointinfo = _assets->get<scene2::SceneNode>("playerpoints");
    _playerpointinfo->setScale(1 / _cameraZoom);
    _playerpointinfo->doLayout(); // Repositions the HUD
    _playerpointinfo->setVisible(false);
    _uinode->addChild(_playerpointinfo);
    
    // there are 5 (1-5) rows with 9 (0-8)
    // playerpoints_playerpoints_playerpointinfo_x
    // playerpoints_playerpoints_playerpointinfo_x_points_pointx
    for (int ii = 1; ii <= 5; ii++) {
        std::shared_ptr<scene2::SceneNode> wholenode = _assets->get<scene2::SceneNode>("playerpoints_playerpoints_playerpointinfo_" + std::to_string(ii));
        std::vector<std::shared_ptr<scene2::SceneNode>> elements;
        for (int jj = 0; jj < 9; jj++) {
            std::shared_ptr<scene2::SceneNode> temp = _assets->get<scene2::SceneNode>("playerpoints_playerpoints_playerpointinfo_" + std::to_string(ii) + "_points_point" + std::to_string(jj));
            elements.push_back(temp);
        }
        _points.insert({wholenode, elements});
    }
    
    _exitbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("playerpoints_exit"));
    _exitbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            // TODO: disconnect to the main menu
        }
    });
    _nextroundbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("playerpoints_next"));
    _nextroundbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            // TODO: set a flag so that you can send an event that you are ready from GameScene
            _nextRound = true;
        }
    });
    
    _nextRound = false;
    
    return true;
}


void UIController::updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick) {
    _joyback->setPosition(joyStick.first / _cameraZoom - _offset / _cameraZoom);
    Vec2 resultant = joyStick.second - joyStick.first;
    float rLength = resultant.length();
    if (rLength / _cameraZoom > _joyback->getWidth() / 2) {
        joyStick.second.set(joyStick.first + resultant.normalize() * _joyback->getWidth() * _cameraZoom / 2);
        // Multiply _joyback->getWidth() by _cameraZoom because the joyback width was already divided by _cameraZoom earlier, and this
        // is still pre-zoom coordinates
    }
    _joymain->setPosition(joyStick.second / _cameraZoom - _offset / _cameraZoom);
}

std::list<cugl::Vec2> UIController::getAdjustedSwipePoints() {
    std::shared_ptr<std::list<std::pair<cugl::Vec2, cugl::Timestamp>>> sp = _input->getSwipePoints();
    std::list<cugl::Vec2> adjustedPoints;
    for (auto it = sp->begin(); it != sp->end();) {
        adjustedPoints.push_back((it->first - _offset) / _cameraZoom);
        it++;
    }
    return adjustedPoints;
}

std::vector<cugl::Vec2> UIController::computeTriangulatedPoints() {
//    std::list<cugl::Vec2> swipePoints = _input->getSwipePoints();
    std::list<cugl::Vec2> swipePoints = getAdjustedSwipePoints();
    std::vector<cugl::Vec2> points;
    float thickness = swipeThickness;
    float pSize = swipePoints.size();
    float i = 0;
    auto it = swipePoints.begin();
    points.push_back(*it);
    while (it != (std::prev(std::prev(swipePoints.end())))) {
        auto a = *it;
        it++;
        auto b = *it;
        Vec2::subtract(a, b, &tmp);
        tmp.normalize();
        tmp.set(-tmp.y, tmp.x);
        thickness = swipeThickness * ((pSize - i) / pSize);
        points.push_back(Vec2(b - tmp * thickness/2));
        points.push_back(Vec2(b + tmp * thickness/2));
        i++;
    }
    it++;
    points.push_back(*it);
    return points;
}

std::vector<Uint32> UIController::computeTriangulatedIndices(int numTriangles) {
    std::vector<Uint32> indices;
    for (int i = 0; i < numTriangles; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }
    return indices;
}

cugl::Color4 UIController::getSwipeColorForInput() {
    if (_input->didDashThisSwipe()) {
        return Color4::ORANGE;
    }
//    else if (_character->isRootingUnrooting()) { // TODO: Blue should only show up when we CAN actually root/unroot. Need a way to get this check from the game state
    else if (_input->didRootNow()) { // temporary for demo purposes
        return Color4::BLUE;
    }
    return Color4::WHITE;
}

void UIController::updateSwipeSpline() { // div by cameraZoom and offset
    _input->cullSwipePointsByDuration();
    int numSwipePoints = _input->getSwipePoints()->size();
    if (numSwipePoints > 2) {
        std::vector<cugl::Vec2> swipePointsTri = computeTriangulatedPoints();
        auto poly = Poly2(swipePointsTri, computeTriangulatedIndices(swipePointsTri.size()-2));
        _swipeNode->setPolygon(poly);
        _swipeNode->setPosition(poly.getBounds().origin);
        _swipeNode->setVisible(true);
        _swipeNode->setColor(getSwipeColorForInput());
    }
    else {
        _swipeNode->setVisible(false);
    }
}

void UIController::updateInfoNodes(int numCarrots, int numBarrots) {
    _carrotsRemainingText->setText("remaining player carrots: " + std::to_string(numCarrots));
    _barrotsRemainingText->setText("remaining baby carrots: " + std::to_string(numBarrots));
}

void UIController::updateDashTimerNode(bool canDash) {
    _dashTimerNode->setColor(canDash ? Color4::GREEN : Color4::RED);
}

void UIController::update(float step, std::shared_ptr<OrthographicCamera> camera, int numCarrots, int numBarrots, bool debugActive, bool canDash) {
    _cameraZoom = camera->getZoom();
    _uinode->setPosition(camera->getPosition() - Vec2(SCENE_WIDTH, SCENE_HEIGHT)/2/_cameraZoom);
    if (_input->withJoystick()) {
        _joymain->setVisible(true);
        _joyback->setVisible(true);
        updateJoystick(_input->getJoystick());
    } else {
        _joymain->setVisible(false);
        _joyback->setVisible(false);
    }
    _carrotsRemainingBoard->setVisible(debugActive);
    _barrotsRemainingBoard->setVisible(debugActive);
    updateSwipeSpline();
    updateInfoNodes(numCarrots, numBarrots);
    updateDashTimerNode(canDash);
    
    if (_dialogBoxVisible) {
        _dialogBox->setScale(EasingFunction::elasticOut(_easingtime * 3, 0.7) * _dialogBoxFullScale);
    } else {
        _dialogBox->setScale(std::max((1.0f - EasingFunction::expoOut(_easingtime * 10)) * _dialogBoxFullScale, 0.0f));
    }
    _easingtime += step;
}
