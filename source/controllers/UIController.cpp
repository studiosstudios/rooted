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

void UIController::cullSwipePointsByDuration() {
    std::shared_ptr<std::list<std::pair<cugl::Vec2, cugl::Timestamp>>> sp = _input->getSwipePoints();
    cugl::Timestamp curTime = cugl::Timestamp();
    for (auto it = sp->begin(); it != sp->end();) {
        it = (it->second + swipeDurationMillis < curTime) ? sp->erase(it) : ++it;
    }
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

void UIController::updateSwipeSpline() { // div by cameraZoom and offset
    cullSwipePointsByDuration();
    int numSwipePoints = _input->getSwipePoints()->size();
    if (numSwipePoints > 2) {
//        std::list<cugl::Vec2> swipePoints = _input->getSwipePoints();
        std::vector<cugl::Vec2> swipePointsTri = computeTriangulatedPoints();
        auto poly = Poly2(swipePointsTri, computeTriangulatedIndices(swipePointsTri.size()-2));
        _swipeNode->setPolygon(poly);
        _swipeNode->setPosition(poly.getBounds().origin);
        _swipeNode->setVisible(true);
        _swipeNode->setColor(_input->getCurrentSwipeColor());
    }
    else {
        _swipeNode->setVisible(false);
    }
}

void UIController::updateInfoNodes(int numCarrots, int numBarrots) {
    _carrotsRemainingText->setText("remaining player carrots: " + std::to_string(numCarrots));
    _barrotsRemainingText->setText("remaining baby carrots: " + std::to_string(numBarrots));
}

void UIController::update(float step, std::shared_ptr<OrthographicCamera> camera, int numCarrots, int numBarrots, bool debugActive) {
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
}
