//
//  FreeEvent.cpp
//  Rooted
//
//  Created by Kimmy Lin on 4/18/24.
//

#include "FreeEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> FreeEvent::newEvent(){
    return std::make_shared<FreeEvent>();
}

std::shared_ptr<NetEvent> FreeEvent::allocFreeEvent(std::string uuid){
#pragma mark BEGIN SOLUTION
    auto event = std::make_shared<FreeEvent>();
    event->_uuid = uuid;
    return event;
#pragma mark END SOLUTION
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> FreeEvent::serialize(){
#pragma mark BEGIN SOLUTION
    _serializer.reset();
    _serializer.writeString(_uuid);
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
void FreeEvent::deserialize(const std::vector<std::byte>& data){
#pragma mark BEGIN SOLUTION
    _deserializer.reset();
    _deserializer.receive(data);
    std::string uuid = _deserializer.readString();
    _uuid = uuid;
#pragma mark END SOLUTION
}
