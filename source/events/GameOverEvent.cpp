//
//  Gameover.cpp
//  Rooted
//
//  Created by Kimmy Lin on 3/20/24.
//

#include "GameOverEvent.h"

/**
 * This method is used by the NetEventController to create a new event of using a
 * reference of the same type.
 *
 * Not that this method is not static, it differs from the static alloc() method
 * and all methods must implement this method.
 */
std::shared_ptr<NetEvent> GameOverEvent::newEvent(){
    return std::make_shared<GameOverEvent>();
}

std::shared_ptr<NetEvent> GameOverEvent::allocGameOverEvent(std::string roomid){
    //TODO: make a new shared copy of the event and set its _pos to pos.
#pragma mark BEGIN SOLUTION
    auto event = std::make_shared<GameOverEvent>();
    event->_roomid = roomid;
    return event;
#pragma mark END SOLUTION
}

/**
 * Serialize any paramater that the event contains to a vector of bytes.
 */
std::vector<std::byte> GameOverEvent::serialize(){
    //TODO: serialize _pos
#pragma mark BEGIN SOLUTION
    _serializer.reset();
    _serializer.writeString(_roomid);
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
void GameOverEvent::deserialize(const std::vector<std::byte>& data){
    //TODO: deserialize data and set _pos
    //NOTE: You might be tempted to write Vec2(_deserializer.readFloat(),_deserializer.readFloat()), however, C++ doesn't specify the order in which function arguments are evaluated, so you might end up with <y,x> instead of <x,y>.
    
#pragma mark BEGIN SOLUTION
    _deserializer.reset();
    _deserializer.receive(data);
    std::string roomid = _deserializer.readString();
    _roomid = roomid;
#pragma mark END SOLUTION
}
