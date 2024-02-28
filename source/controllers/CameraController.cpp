//
//  CameraController.cpp
//  Rooted
//
//  Created by Kimmy Lin on 2/28/24.
//

#include "CameraController.h"

using namespace cugl;

#pragma mark -
#pragma mark Initializers

bool CameraController::init(const std::shared_ptr<EntityModel> target, const std::shared_ptr<cugl::scene2::SceneNode> root, float lerp, std::shared_ptr <cugl::OrthographicCamera> camera, std::shared_ptr<scene2::SceneNode> ui, float maxZoom) {
    _target = target;
    _root = root;
    _lerp = lerp;
    _camera = camera;
    _maxZoom = maxZoom;
    _ui = ui;

    return true;
}

#pragma mark -
#pragma mark Camera Handling

void CameraController::update(float dt) {
    Vec2 cameraPos = Vec2(_camera->getPosition().x, _camera->getPosition().y);

    Vec2 target;
    Vec2* dst = new Vec2();
    // Lazily track the target using lerp
    target = Vec2(_target->getX(), _target->getY());
    Vec2::lerp(cameraPos, target, _lerp * dt, dst);
    // Make sure the camera never goes outside of the _root node's bounds
    (*dst).x = std::max(std::min(_root->getSize().width - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), (*dst).x), _camera->getViewport().getMaxX() / (2 * _camera->getZoom()));
    (*dst).y = std::max(std::min(_root->getSize().height - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()), (*dst).y), _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
    _camera->translate((*dst).x - cameraPos.x, (*dst).y - cameraPos.y);
    delete dst;

    _camera->update();
    
    Vec2 uiPos = Vec2(_camera->getPosition().x - _camera->getViewport().getMaxX() / (2 * _camera->getZoom()), _camera->getPosition().y - _camera->getViewport().getMaxY() / (2 * _camera->getZoom()));
    _ui->setPosition(uiPos);
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
    _ui->setScale(1 / _camera->getZoom());
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
    _ui->setScale(1 / _camera->getZoom());
}

void CameraController::setPosition(Vec3 pos){
    _camera->setPosition(pos);
}

void CameraController::setTarget(std::shared_ptr<EntityModel> target) {
    _target = target;
}
