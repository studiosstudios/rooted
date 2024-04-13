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
}

void UIController::initJoystickNodes() {
    _joynode = scene2::SceneNode::allocWithBounds(Vec2(0.35f * 1024 / _cameraZoom, 0.5f * 576 / _cameraZoom) + _offset);
    
    _joyback = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 64));
    _joyback->setColor(Color4(0, 0, 0, 100));
    _joynode->addChild(_joyback);
    
    _joymain = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 32));
    _joymain->setVisible(false);
    _joynode->addChild(_joymain);
    
    _uinode->addChild(_joynode);
}

bool UIController::init(const std::shared_ptr<cugl::scene2::SceneNode> uinode,
                        Vec2 offset, float cameraZoom) {
    _uinode = uinode;
    _offset = offset;
    _cameraZoom = cameraZoom;
    
    initJoystickNodes();
    return true;
}


void UIController::updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick) {
    _joyback->setPosition(joyStick.first / _cameraZoom - _offset / _cameraZoom);
    _joymain->setPosition(joyStick.second / _cameraZoom - _offset / _cameraZoom);
}

void UIController::update(float step, std::shared_ptr<OrthographicCamera> camera,
                          bool joyOn,
                          std::pair<cugl::Vec2, cugl::Vec2> joyStick) {
    _cameraZoom = camera->getZoom();
    _uinode->setPosition(camera->getPosition() - Vec2(1024, 576)/2/_cameraZoom);
    if (joyOn) {
        _joymain->setVisible(true);
        _joyback->setVisible(true);
        updateJoystick(joyStick);
    } else {
        _joymain->setVisible(false);
        _joyback->setVisible(false);
    }
}
