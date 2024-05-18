//
//  Gameover.hpp
//  Rooted
//
// ONLY FOR DEBUGGING
//  Created by Kimmy Lin on 3/20/24.
//

#ifndef Gameover_h
#define Gameover_h

#include <cugl/cugl.h>
using namespace cugl::physics2::net;
using namespace cugl::net;
using namespace cugl;

class ResetEvent : public NetEvent {
    
protected:
    NetcodeSerializer _serializer;
    NetcodeDeserializer _deserializer;
    
    std::string _roomid;
    
    int _type;
    
public:
    /**
    * This method is used by the NetEventController to create a new event of using a
    * reference of the same type.
    *
    * Not that this method is not static, it differs from the static alloc() method
    * and all methods must implement this method.
    */
   std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocResetEvent(int type);
    
    /**
     * Serialize any parameter that the event contains to a vector of bytes.
     */
    std::vector<std::byte> serialize() override;
    /**
     * Deserialize a vector of bytes and set the corresponding parameters.
     *
     * @param data  a byte vector packed by serialize()
     *
     * This function should be the "reverse" of the serialize() function: it
     * should be able to recreate a serialized event entirely, setting all the
     * useful parameters of this class.
     */
    void deserialize(const std::vector<std::byte>& data) override;
    
//    std::string getRoomId() { return _roomid; }
    int getType() {return _type;}
};

#endif /* Gameover_h */
