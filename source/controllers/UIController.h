//
//  UIController.h
//  Rooted
//
//  Created by Choong Jae Lee on 2/27/24.
//

#ifndef ROOTED_UICONTROLLER_H
#define ROOTED_UICONTROLLER_H

#include <cugl/cugl.h>

#endif /* UIController_h */

class UIController {
private:
    cugl::PolyFactory _pf;
    /** Reference to the UI root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _uinode;
    
    std::shared_ptr<cugl::scene2::SceneNode> _joynode;
    
    std::shared_ptr<cugl::scene2::PolygonNode> _joyback;
    std::shared_ptr<cugl::scene2::PolygonNode> _joymain;
    
public:
    UIController() {};
    
    ~UIController() { dispose(); }
    
    void dispose();
    
    void initJoystickNodes();
    
    bool init(const std::shared_ptr<cugl::scene2::SceneNode> uinode);
    
    void updateJoystick();
    
    void update(float dt);
};
