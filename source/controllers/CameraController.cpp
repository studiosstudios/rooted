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

bool CameraController::init(const std::shared_ptr<EntityModel> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr <cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom, float scale, Vec2 frac) {
    _target = target;
    _root = root;
    _lerp = lerp;
    _camera = camera;
    _maxZoom = maxZoom;
    _ui = ui;
    _frac = frac;
    _scale = scale;

    return true;
}

#pragma mark -
#pragma mark Camera Handling

void CameraController::update(float dt) {
    float curr_x = _camera->getPosition().x;
    float curr_y = _camera->getPosition().y;
    Vec2 newPos = boundPosition(_target->getPosition()*_scale);
    _camera->setPosition(Vec3(curr_x + (newPos.x-curr_x) * _lerp, curr_y + (newPos.y-curr_y) * _lerp, _camera->getPosition().z));
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

void CameraController::addZoom(float zoom) {
    float originalZoom = _camera->getZoom();
    // Don't let it be greater than max zoom
    if (originalZoom + zoom > _maxZoom) return;
    float truezoom = std::max(originalZoom + zoom, 0.01f);
    _camera->setZoom(truezoom);
    // If this causes the camera to go out of bounds, revert the change
    if (_root->getSize().width < _camera->getViewport().getMaxX() / _camera->getZoom() || _root->getSize().height < _camera->getViewport().getMaxY() / _camera->getZoom()) {
        _camera->setZoom(originalZoom);
    }
    // Scale the UI so that it always looks the same size
//    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setPosition(Vec3 pos){
    _camera->setPosition(boundPosition(pos));
}

const Vec2 CameraController::boundPosition(Vec2 pos) {
    int viewWidth = _camera->getViewport().getMaxX();
    int viewHeight = _camera->getViewport().getMaxY();
    float new_x = std::min(std::max(pos.x, (float) ((viewWidth/2)/_camera->getZoom())), (float) (SCENE_WIDTH * _frac.x-(viewWidth/2)/_camera->getZoom()));
    float new_y = std::min(std::max(pos.y, (float) ((viewHeight/2)/_camera->getZoom())), (float) (SCENE_HEIGHT  * _frac.y-(viewHeight/2)/_camera->getZoom()));
    return Vec2(new_x, new_y);
}

void CameraController::setTarget(std::shared_ptr<EntityModel> target) {
    _target = target;
}
