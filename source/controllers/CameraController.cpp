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
    _pos = _camera->getPosition();
    _defaultZoom = DEFAULT_CAMERA_ZOOM*DEFAULT_DRAWSCALE/_scale;
    _zoomTarget = _defaultZoom;
    _shake = 0;
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
        if(std::abs(_camera->getZoom() - _zoomTarget) > ZOOM_ERR){
            zoomIn((_zoomTarget-_camera->getZoom())*ZOOM_RATE);
        }
        else {
            _camera->setZoom(_zoomTarget);
        }
    }
    if(std::abs(newPos.x-curr_x) < CAM_POSITION_ERR && std::abs(newPos.y-curr_y) < CAM_POSITION_ERR){
        _pos.set(newPos);
    }
    else {
        _pos += (newPos - _pos) * _lerp;
    }
    setPosition(_pos, false);
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
    _camera->setZoom(originalZoom + zoom);
    // If this causes the camera to go out of bounds, revert the change
    if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
        _camera->setZoom(originalZoom);
    }
    // Scale the UI so that it always looks the same size
//    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setPosition(Vec3 pos, bool setTarget){
    if (setTarget) {
        _posTarget.x = pos.x;
        _posTarget.y = pos.y;
    }
    Vec2 rand = Vec2((((float) std::rand()/RAND_MAX) * 2.0 - 1) * _shake, (((float) std::rand()/RAND_MAX) * 2.0 - 1) * _shake);
    _camera->setPosition(boundPosition(pos) + rand);
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
