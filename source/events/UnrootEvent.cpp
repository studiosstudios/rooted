//
//  UnrootEvent.cpp
//  Rooted
//
//  Created by Kimmy Lin on 3/19/24.
//

#include "UnrootEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> UnrootEvent::newEvent(){
    return std::make_shared<UnrootEvent>();
}

std::shared_ptr<NetEvent> UnrootEvent::allocUnrootEvent(std::string uuid, int plantingspotid){
#pragma mark BEGIN SOLUTION
    auto event = std::make_shared<UnrootEvent>();
    event->_uuid = uuid;
    event->_plantingspotid = plantingspotid;
    return event;
#pragma mark END SOLUTION
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> UnrootEvent::serialize(){
    //TODO: serialize _pos
#pragma mark BEGIN SOLUTION
    _serializer.reset();
    _serializer.writeString(_uuid);
    _serializer.writeSint32(_plantingspotid);
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
void UnrootEvent::deserialize(const std::vector<std::byte>& data){
#pragma mark BEGIN SOLUTION
    _deserializer.reset();
    _deserializer.receive(data);
    std::string uuid = _deserializer.readString();
    _uuid = uuid;
    int plantingspotid = _deserializer.readSint32();
    _plantingspotid = plantingspotid;
#pragma mark END SOLUTION
}

