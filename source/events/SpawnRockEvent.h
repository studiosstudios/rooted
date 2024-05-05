//
// Created by Jeffrey Zhou on 4/29/24.
//

#ifndef ROOTED_SPAWNROCKEVENT_H
#define ROOTED_SPAWNROCKEVENT_H
#include <cugl/cugl.h>

using namespace cugl::physics2::net;
using namespace cugl::net;
using namespace cugl;

class SpawnRockEvent : public NetEvent {

protected:
    NetcodeSerializer _serializer;
    NetcodeDeserializer _deserializer;

    Vec2 _pos;
    int _idx;
    Vec2 _vel;
    std::string _uuid;

public:
    /**
    * This method is used by the NetEventController to create a new event of using a
    * reference of the same type.
    *
    * Not that this method is not static, it differs from the static alloc() method
    * and all methods must implement this method.
    */
    std::shared_ptr<NetEvent> newEvent() override;

    static std::shared_ptr<NetEvent> allocSpawnRockEvent(Vec2 pos, int idx, Vec2 vel, std::string uuid);

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

    Vec2 getPosition() { return _pos; }
    
    Vec2 getVelocity() { return _vel; }
    
    int getIndex() { return _idx; }
    
    std::string getUUID() { return _uuid; }
};


#endif //ROOTED_SPAWNROCKEVENT_H
