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
    
    cugl::Vec2 _offset;
    
public:
    UIController() {};
    
    ~UIController() { dispose(); }
    
    void dispose();
    
    void initJoystickNodes();
    
    bool init(const std::shared_ptr<cugl::scene2::SceneNode> uinode,
              cugl::Vec2 offset);
    
    void updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick);
    
    void update(float step, std::shared_ptr<cugl::OrthographicCamera> &camera, bool joyOn, std::pair<cugl::Vec2, cugl::Vec2> joyStick);
};
