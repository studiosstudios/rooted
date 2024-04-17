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
    /** PolyFactory for drawing the joystick circle.
     *  TODO: We can remove this once we get proper assets for the joystick
     */
    cugl::PolyFactory _pf;
    
    /** Pointer to the UI root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _uinode;
    
    /** Pointer to the SceneNode which is the root of the joystick UI */
    std::shared_ptr<cugl::scene2::SceneNode> _joynode;
    
    /** Pointer to the PolygonNode of the joystick's backing circle*/
    std::shared_ptr<cugl::scene2::PolygonNode> _joyback;
    /** Pointer to the PolygonNode of the joystick's moving circle*/
    std::shared_ptr<cugl::scene2::PolygonNode> _joymain;
    
    /** The offset of the scene graph
     *
     *  This offset is nonzero if the device's screen is not 1024 x 576.
     *  In which case, either the X xor Y coordinate will be non-zero.
     */
    cugl::Vec2 _offset;
    
    /** The camera's zoom */
    float _cameraZoom;
    
public:
    UIController() {};
    
    ~UIController() { dispose(); }
    
    void dispose();
    
    void initJoystickNodes();
    
    bool init(const std::shared_ptr<cugl::scene2::SceneNode> uinode,
              cugl::Vec2 offset, float cameraZoom);
    
    void updateJoystick(std::pair<cugl::Vec2, cugl::Vec2> joyStick);
    
    void update(float step, std::shared_ptr<cugl::OrthographicCamera> camera, bool joyOn, std::pair<cugl::Vec2, cugl::Vec2> joyStick);
};
