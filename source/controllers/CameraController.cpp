//
//  CameraController.cpp
//  Rooted
//
//  Created by Kimmy Lin on 2/28/24.
//

#include "CameraController.h"
#include "../RootedConstants.h"

using namespace cugl;

#pragma mark -
#pragma mark Initializers

bool CameraController::init(const std::shared_ptr<EntityModel> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr <cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom, float scale) {
    _target = target;
    _root = root;
    _lerp = lerp;
    _camera = camera;
    _maxZoom = maxZoom;
    _ui = ui;
    _scale = scale;
    _startTimer = HOLD_CAM; //DEAL WITH THIS IN ACTUAL CODE

    return true;
}

#pragma mark -
#pragma mark Camera Handling

void CameraController::update(float dt) {
    int viewWidth = _camera->getViewport().getMaxX();
    int viewHeight = _camera->getViewport().getMaxY();
    float new_x = std::min(std::max((_target->getPosition()*_scale).x, (float) ((viewWidth/2)/_camera->getZoom())), (float) (SCENE_WIDTH-(viewWidth/2)/_camera->getZoom()));
    float new_y = std::min(std::max((_target->getPosition()*_scale).y, (float) ((viewHeight/2)/_camera->getZoom())), (float) (SCENE_HEIGHT-(viewHeight/2)/_camera->getZoom()));
    float curr_x = _camera->getPosition().x;
    float curr_y = _camera->getPosition().y;
    if(_startTimer > 0){
        _startTimer--;
    }
    if(_startTimer == 0){
        if(_camera->getZoom() < DEFAULT_CAMERA_ZOOM-ZOOM_ERR){
            _camera->setPosition(Vec3(new_x, new_y, _camera->getPosition().z));
            zoomIn((DEFAULT_CAMERA_ZOOM-_camera->getZoom())*ZOOM_RATE);
        }
        else if (_camera->getZoom() < DEFAULT_CAMERA_ZOOM){
            _camera->setZoom(DEFAULT_CAMERA_ZOOM);
            _camera->setPosition(Vec3(new_x, new_y, _camera->getPosition().z));
        }
    }
    if(std::abs(new_x-curr_x) < CAM_POSITION_ERR && std::abs(new_x-curr_x) < CAM_POSITION_ERR){
        _camera->setPosition(Vec3(new_x, new_y, _camera->getPosition().z));
    }
    else {
        _camera->setPosition(Vec3(curr_x + (new_x-curr_x) * _lerp, curr_y + (new_y-curr_y) * _lerp, _camera->getPosition().z));
    }
    _camera->update();
//    Vec2 uiPos = Vec2(_camera->getPosition().x - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
//    _ui->setPosition(uiPos);
}

void CameraController::setZoom(float zoom) {
    float originalZoom = _camera->getZoom();
    // Don't let it be greater than max zoom
    if (zoom > _maxZoom) return;
    _camera->setZoom(zoom);
    // If this causes the camera to go out of bounds, revert the change
    if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
        _camera->setZoom(originalZoom);
    }
    // Scale the UI so that it always looks the same size
//    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::zoomIn(float zoom) {
    float originalZoom = _camera->getZoom();
    // Don't let it be greater than max zoom
    if (originalZoom + zoom > _maxZoom) return;
    float truezoom = std::min(originalZoom + zoom, 2.0f);
    _camera->setZoom(truezoom);
    // If this causes the camera to go out of bounds, revert the change
    if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
        _camera->setZoom(originalZoom);
    }
    // Scale the UI so that it always looks the same size
//    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setPosition(Vec3 pos){
    int viewWidth = _camera->getViewport().getMaxX();
    int viewHeight = _camera->getViewport().getMaxY();
    float new_x = std::min(std::max(pos.x, (float) ((viewWidth/2)/_camera->getZoom())), (float) (SCENE_WIDTH-(viewWidth/2)/_camera->getZoom()));
    float new_y = std::min(std::max(pos.y, (float) ((viewHeight/2)/_camera->getZoom())), (float) (SCENE_HEIGHT-(viewHeight/2)/_camera->getZoom()));
    _camera->setPosition(Vec3(new_x, new_y, pos.z));
}

void CameraController::setTarget(std::shared_ptr<EntityModel> target) {
    _target = target;
}
