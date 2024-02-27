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
    _joynode = scene2::SceneNode::alloc();
    
    _joyback = scene2::PolygonNode::allocWithPoly(_pf.makeCircle(Vec2(0,0), 32));
    _joyback->setAnchor(Vec2::ANCHOR_CENTER);
    _joynode->addChild(_joyback);
    
    _uinode->addChild(_joynode);
}

bool UIController::init(const std::shared_ptr<cugl::scene2::SceneNode> uinode) {
    _uinode = uinode;
    
    initJoystickNodes();
    return true;
}


void updateJoystick() {
    
}


void update(float dt) {
    updateJoystick();
}
