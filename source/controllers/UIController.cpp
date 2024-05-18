//
//  UIController.cpp
//  Rooted
//
//  Created by Choong Jae Lee on 2/27/24.
//
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "UIController.h"
#include "../RootedConstants.h"

using namespace cugl;

void UIController::dispose() {
    _uinode->removeAllChildren();
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
    _nextRound = 0;
    if (visible) {
        _nextbutton->activate();
    } else {
        _nextbutton->deactivate();
        _nextbutton->setDown(false);
        _exitbutton->deactivate();
        _exitbutton->setDown(false);
        _nextroundbutton->deactivate();
        _nextroundbutton->setDown(false);
    }
}

void UIController::setEndVariables(int roundNum, int length, int babies, std::vector<int> carrots, std::vector<int> pointsVec, int playerNum) {
    _roundNumber->setText("ROUND " + std::to_string(roundNum));
    
    int seconds = length / 1000;
    int minutes = seconds / 60;
    seconds %= 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    std::string result = oss.str();
    _time->setText(result);
    
    _babyCarrots->setText(std::to_string(babies));
    
    _notRootedLabel->setVisible(carrots.size() == 0);
    
    for (int ii = 0; ii < _carrotsRooted.size(); ii++) {
        _carrotsRooted.at(ii)->setVisible(std::find(carrots.begin(), carrots.end(), ii) != carrots.end());
    }
    
    _rootedLabel->setVisible(std::find(carrots.begin(), carrots.end(), playerNum) != carrots.end());
    
    auto temp =_uinode->getChildByName("playerpoints")->getChildByName("playerpoints");
    // set all to not be visible first
    for (int ii = 0; ii < _points.size(); ii++) {
        temp->getChildByName("playerpointinfo_" + std::to_string(ii))->setVisible(false);
    }
    for (int ii = 0; ii < pointsVec.size(); ii++) {
        auto temp2 = temp->getChildByName("playerpointinfo_" + std::to_string(ii));
        temp2->setVisible(true);
        
        for (int jj = 0; jj < pointsVec.at(ii); jj++) {
            if (jj < 10) {
                _points.at(temp2).at(jj * 2)->setVisible(false);
                _points.at(temp2).at(jj * 2 + 1)->setVisible(true);
            }
        }
    }
}

void UIController::setCharacterDisplay(bool active, int carrot) {
    _characterdisplay->setVisible(active);
    for (auto it = _characterdisplayNodes.begin(); it != _characterdisplayNodes.end(); ++it) {
        (*it)->setVisible(false);
    }
    _characterdisplayNodes.at(carrot)->setVisible(true);
}

void UIController::setRabbitPreview(bool active) {
    _rabbitpreview->setVisible(active);
}

void UIController::setCarrotPreview(bool active, int carrot) {
    _carrotpreview->setVisible(active);
    for (auto it = _carrotpreviewNodes.begin(); it != _carrotpreviewNodes.end(); ++it) {
        (*it)->setVisible(false);
    }
    _carrotpreviewNodes.at(carrot)->setVisible(true);
}

void UIController::setWinnerDisplay(bool active, int carrot) {
    _winner->setVisible(active);
    for (auto it = _winnerNodes.begin(); it != _winnerNodes.end(); ++it) {
        (*it)->setVisible(false);
    }
    _winnerNodes.at(carrot)->setVisible(true);
    if (active) {
        _exitbutton->activate();
        _winnernextbutton->activate();
    } else {
        _exitbutton->deactivate();
        _exitbutton->setDown(false);
        _winnernextbutton->deactivate();
        _winnernextbutton->setDown(false);
    }
}

void UIController::setSpeechBubbleVisible(bool visible) {
    _speechBubble->setVisible(visible);
}

void UIController::setDialogBoxVisible(bool visible) {
    _dialogBox->setVisible(visible);
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
    _speechBubble = scene2::NinePatch::allocWithTexture(_assets->get<Texture>("speechbubble"), Rect(137, 666, 512, 512));
    _speechBubble->SceneNode::setContentSize(Size(1000, 500));
    _speechBubble->setPosition((Vec2(SCENE_WIDTH/2 - 300, SCENE_HEIGHT/2 - 100)) / _cameraZoom);
    _speechBubble->setAnchor(Vec2::ANCHOR_CENTER);
    _speechBubble->setScale(0.18 * _drawScale/DEFAULT_DRAWSCALE);
    _speechBubble->setVisible(false);
    _uinode->addChild(_speechBubble);
    
    _speechBubbleText = scene2::Label::allocWithText("Oh no, all of my baby carrots have escaped!", _assets->get<Font>("gaeguBold75"));
    _speechBubbleText->setHorizontalAlignment(HorizontalAlign::CENTER);
    _speechBubbleText->setVerticalAlignment(VerticalAlign::TOP);
    _speechBubbleText->setPadding(80, 0, 80, 0);
    _speechBubbleText->setContentSize(_speechBubble->getContentSize());
    _speechBubbleText->doLayout();
    _speechBubbleText->setWrap(true);
    _speechBubble->addChild(_speechBubbleText);

    _dialogBox = scene2::NinePatch::allocWithTexture(_assets->get<Texture>("dialoguebox"), Rect(137, 666, 512, 512));
    _dialogBox->SceneNode::setContentSize(Size(1300, 500));
    _dialogBox->setPosition((Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2)) / _cameraZoom);
    _dialogBox->setAnchor(Vec2::ANCHOR_CENTER);
    _dialogBox->setScale(0.18 * _drawScale/DEFAULT_DRAWSCALE);
    _dialogBox->setVisible(false);
    _uinode->addChild(_dialogBox);
    
    _dialogBoxText = scene2::Label::allocWithText("Drag left side of screen to move. Swipe in a direction to dash. \n Capture all of the baby carrots!", _assets->get<Font>("gaeguBold75"));
    _dialogBoxText->setHorizontalAlignment(HorizontalAlign::CENTER);
    _dialogBoxText->setVerticalAlignment(VerticalAlign::TOP);
    _dialogBoxText->setPadding(200, 0, 200, 0);
    _dialogBoxText->setContentSize(_dialogBox->getContentSize());
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
    
    initJoystickNodes();
    initGameUINodes();
    
    _swipeNode = scene2::PolygonNode::alloc();
    _swipeNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _uinode->addChild(_swipeNode);
    
    // post round scene info
    _postroundscene = _assets->get<scene2::SceneNode>("postround");
    _postroundscene->setScale(1 / _cameraZoom);
    _postroundscene->doLayout(); // Repositions the HUD
    _postroundscene->setVisible(false);
    _uinode->addChild(_postroundscene);
    
    _roundNumber = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_ROUNDNUM"));
    _time = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_gamelength_gametime"));
    _babyCarrots = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_babycarrotscount_numbabycarrots"));
    _carrotsRooted = {
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carroticons_Big_Icon_1"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carroticons_Big_Icon_2"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carroticons_Big_Icon_3"),
        _assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_carroticons_Big_Icon_4"),
    };
    _rootedLabel = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_rootedcarrots"));
    _notRootedLabel = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("postround_stats_rootedcarrots_nonerooted"));
    
    _nextbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("postround_next"));
    _nextbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            CULog("this button was pressed");
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
    
    // there are 4 (0-3) rows with 10 (0-9)
    // playerpoints_playerpoints_playerpointinfo_x
    // playerpoints_playerpoints_playerpointinfo_x_emptypoints_emptypoint_x
    // playerpoints_playerpoints_playerpointinfo_x_fullpoints_fullpoint_x
    _points.clear();
    for (int ii = 0; ii < 4; ii++) {
        std::shared_ptr<scene2::SceneNode> wholenode = _assets->get<scene2::SceneNode>("playerpoints_playerpoints_playerpointinfo_" + std::to_string(ii));
        std::vector<std::shared_ptr<scene2::SceneNode>> elements;
        for (int jj = 9; jj >= 0; jj--) {
            std::shared_ptr<scene2::SceneNode> temp = _assets->get<scene2::SceneNode>("playerpoints_playerpoints_playerpointinfo_" + std::to_string(ii) + "_emptypoints_emptypoint_" + std::to_string(jj));
            elements.push_back(temp);
            std::shared_ptr<scene2::SceneNode> temp2 = _assets->get<scene2::SceneNode>("playerpoints_playerpoints_playerpointinfo_" + std::to_string(ii) + "_fullpoints_fullpoint_" + std::to_string(jj));
            elements.push_back(temp2);
        }
        _points.insert({wholenode, elements});
    }
    
    _nextroundbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("playerpoints_next"));
    _nextroundbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            _nextRound = 1;
        }
    });
    
    _nextRound = 0;
    
    _characterdisplay = _assets->get<scene2::SceneNode>("characterdisplay");
    _characterdisplay->setScale(1 / _cameraZoom);
    _characterdisplay->doLayout(); // Repositions the HUD
    _characterdisplay->setVisible(true);
    _uinode->addChild(_characterdisplay);
    
    _rabbitpreview = _assets->get<scene2::SceneNode>("rabbitpreview");
    _rabbitpreview->setScale(1 / _cameraZoom);
    _rabbitpreview->doLayout(); // Repositions the HUD
    _rabbitpreview->setVisible(false);
    _uinode->addChild(_rabbitpreview);
    
    _carrotpreview = _assets->get<scene2::SceneNode>("carrotpreview");
    _carrotpreview->setScale(1 / _cameraZoom);
    _carrotpreview->doLayout(); // Repositions the HUD
    _carrotpreview->setVisible(false);
    _uinode->addChild(_carrotpreview);
    
    _winner = _assets->get<scene2::SceneNode>("winner");
    _winner->setScale(1 / _cameraZoom);
    _winner->doLayout(); // Repositions the HUD
    _winner->setVisible(false);
    _uinode->addChild(_winner);
    
    // winner specific buttons
    _exitbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winner_exit"));
    _exitbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            // TODO: disconnect to the main menu
        }
    });
    _winnernextbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("winner_next"));
    _winnernextbutton->addListener([this](const std::string& name, bool down) {
        if (!down) {
            // TODO: send the next game event
        }
    });
    
    _characterdisplayNodes.clear();
    _carrotpreviewNodes.clear();
    _winnerNodes.clear();
    for (int i = 0; i < 4; i++) {
        _characterdisplayNodes.push_back(_assets->get<scene2::SceneNode>("characterdisplay_Big_Icon_" + std::to_string(i)));
        _carrotpreviewNodes.push_back(_assets->get<scene2::SceneNode>("carrotpreview_Big_Icon_" + std::to_string(i)));
        _winnerNodes.push_back(_assets->get<scene2::SceneNode>("winner_characterdisplay_Big_Icon_" + std::to_string(i)));
    }
    
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
    auto label = std::dynamic_pointer_cast<scene2::Label>(_nextroundbutton->getChildByName("NEXT"));
    if (_nextRound == 2) {
        label->setText("...");
    } else {
        label->setText("NEXT");
    }
    _carrotsRemainingBoard->setVisible(debugActive);
    _barrotsRemainingBoard->setVisible(debugActive);
    updateSwipeSpline();
    updateInfoNodes(numCarrots, numBarrots);
    updateDashTimerNode(canDash);
}
