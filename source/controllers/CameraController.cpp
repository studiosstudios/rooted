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

bool CameraController::init(const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr <cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom, float scale, Rect frac) {
    _root = root;
    _lerp = lerp;
    _camera = camera;
    _maxZoom = maxZoom;
    _ui = ui;
    _frac = frac;
    _scale = scale;
    _startTimer = HOLD_CAM; //DEAL WITH THIS IN ACTUAL CODE

    return true;
}

#pragma mark -
#pragma mark Camera Handling

void CameraController::update(float dt) {
    float curr_x = _camera->getPosition().x;
    float curr_y = _camera->getPosition().y;
    Vec2 newPos = boundPosition(_posTarget);
    if(_startTimer > 0){
        _startTimer--;
    }
    if(_startTimer == 0){
        float targetZoom = DEFAULT_CAMERA_ZOOM*DEFAULT_DRAWSCALE/_scale;
        if(_camera->getZoom() < targetZoom-ZOOM_ERR){
            zoomIn((targetZoom-_camera->getZoom())*ZOOM_RATE);
        }
        else if (_camera->getZoom() < targetZoom){
            _camera->setZoom(targetZoom);
        }
    }
    if(std::abs(newPos.x-curr_x) < CAM_POSITION_ERR && std::abs(newPos.y-curr_y) < CAM_POSITION_ERR){
        _camera->setPosition(Vec3(newPos.x, newPos.y, _camera->getPosition().z));
    }
    else {
        _camera->setPosition(Vec3(curr_x + (newPos.x-curr_x) * _lerp, curr_y + (newPos.y-curr_y) * _lerp, _camera->getPosition().z));
    }
    _camera->update();
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
    float truezoom = std::min(originalZoom + zoom, DEFAULT_CAMERA_ZOOM*DEFAULT_DRAWSCALE/_scale);
    _camera->setZoom(truezoom);
    // If this causes the camera to go out of bounds, revert the change
    if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
        _camera->setZoom(originalZoom);
    }
    // Scale the UI so that it always looks the same size
//    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setPosition(Vec3 pos){
    _posTarget.x = pos.x;
    _posTarget.y = pos.y;
    _camera->setPosition(boundPosition(pos));
}

const Vec2 CameraController::boundPosition(Vec2 pos) {
    int viewWidth = _camera->getViewport().getMaxX();
    int viewHeight = _camera->getViewport().getMaxY();
    float new_x = std::clamp(pos.x, float(SCENE_WIDTH * _frac.origin.x + viewWidth/2/_camera->getZoom()),
                             float(SCENE_WIDTH * _frac.getMaxX() - viewWidth/2/_camera->getZoom()));
    float new_y = std::clamp(pos.y, float(SCENE_HEIGHT * _frac.origin.y + viewHeight/2/_camera->getZoom()),
                             float(SCENE_HEIGHT * _frac.getMaxY() - viewHeight/2/_camera->getZoom()));
    return Vec2(new_x, new_y);
}
