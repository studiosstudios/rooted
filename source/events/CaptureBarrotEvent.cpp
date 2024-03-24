//
//  CaptureBarrotEvent.cpp
//  Rooted
//
//  Created by Kimmy Lin on 3/22/24.
//

#include "CaptureBarrotEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> CaptureBarrotEvent::newEvent(){
    return std::make_shared<CaptureBarrotEvent>();
}

std::shared_ptr<NetEvent> CaptureBarrotEvent::allocCaptureBarrotEvent(std::string carrotuuid, int barrotid){
#pragma mark BEGIN SOLUTION
    auto event = std::make_shared<CaptureBarrotEvent>();
    event->_carrotuuid = carrotuuid;
    event->_barrotid = barrotid;
    return event;
#pragma mark END SOLUTION
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> CaptureBarrotEvent::serialize(){
#pragma mark BEGIN SOLUTION
    _serializer.reset();
    _serializer.writeString(_carrotuuid);
    _serializer.writeSint32(_barrotid);
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
void CaptureBarrotEvent::deserialize(const std::vector<std::byte>& data){
#pragma mark BEGIN SOLUTION
    _deserializer.reset();
    _deserializer.receive(data);
    _carrotuuid = _deserializer.readString();
    _barrotid = _deserializer.readSint32();
#pragma mark END SOLUTION
}
