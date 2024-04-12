//
//  RustleEvent.cpp
//  Rooted
//
//  Created by Kimmy Lin on 4/11/24.
//

#include "RustleEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> RustleEvent::newEvent(){
    return std::make_shared<RustleEvent>();
}

std::shared_ptr<NetEvent> RustleEvent::allocRustleEvent(std::string uuid, bool isMoving){
#pragma mark BEGIN SOLUTION
    auto event = std::make_shared<RustleEvent>();
    event->_uuid = uuid;
    event->_isMoving = isMoving;
    return event;
#pragma mark END SOLUTION
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> RustleEvent::serialize(){
#pragma mark BEGIN SOLUTION
    _serializer.reset();
    _serializer.writeString(_uuid);
    _serializer.writeBool(_isMoving);
    return _serializer.serialize();
#pragma mark END SOLUTION
}
/**
 * Deserialize a vector of bytes and set the corresponding parameters.
 *
 * @param data  a byte vector packed by serialize()
 *
 * This function should be the "reverse" of the serialize() function: it
 * should be able to recreate a serialized event entirely, setting all the
 * useful parameters of this class.
 */
void RustleEvent::deserialize(const std::vector<std::byte>& data){
#pragma mark BEGIN SOLUTION
    _deserializer.reset();
    _deserializer.receive(data);
    std::string uuid = _deserializer.readString();
    bool isMoving = _deserializer.readBool();
    _uuid = uuid;
    _isMoving = isMoving;
#pragma mark END SOLUTION
}
