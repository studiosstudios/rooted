//
//  UIController.cpp
//  Rooted
//
//  Created by Choong Jae Lee on 2/27/24.
//

#include "UIController.h"

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
    
    _joyback = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 64));
    _joyback->setColor(Color4(0, 0, 0, 100));
    _joynode->addChild(_joyback);
    
    _joymain = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 32));
    _joymain->setVisible(false);
    _joynode->addChild(_joymain);
    
    _uinode->addChild(_joynode);
}

void UIController::initGameUINodes() {
    _winNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("victory"));
    _winNode->setPosition(Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2) / _cameraZoom + _offset);
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setVisible(false);
    _uinode->addChild(_winNode);
    
    _loseNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("defeat"));
    _loseNode->setPosition(Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2) / _cameraZoom + _offset);
    _loseNode->setAnchor(Vec2::ANCHOR_CENTER);
    _loseNode->setVisible(false);
    _uinode->addChild(_loseNode);
}

bool UIController::init(const std::shared_ptr<cugl::AssetManager>& assets,
                        const std::shared_ptr<InputController>& input,
                        const std::shared_ptr<cugl::scene2::SceneNode> uinode,
                        Vec2 offset, float cameraZoom) {
    _assets = assets;
    _input = input;
    _uinode = uinode;
    _offset = offset;
    _cameraZoom = cameraZoom;
    
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
    }
    else {
        _swipeNode->setVisible(false);
    }
}

void UIController::update(float step, std::shared_ptr<OrthographicCamera> camera) {
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
    updateSwipeSpline();
}
