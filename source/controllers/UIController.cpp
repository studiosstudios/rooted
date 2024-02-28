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
    _joynode = scene2::SceneNode::allocWithBounds(Vec2(0.35f * 1024 / 1.5, 0.5f * 576 / 1.5) + _offset);
    
    _joyback = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 64));
    _joyback->setPosition(_joynode->getWidth()/2, _joynode->getHeight()/2);
    _joynode->addChild(_joyback);
    std::cout << _joynode->getPositionX() << " " << _joynode->getPositionY();
    
    _uinode->addChild(_joynode);
}

bool UIController::init(const std::shared_ptr<cugl::scene2::SceneNode> uinode,
                        Vec2 offset) {
    _uinode = uinode;
    _offset = offset;
    
    initJoystickNodes();
    return true;
}


void updateJoystick() {
    
}


void update(float dt) {
    updateJoystick();
}
