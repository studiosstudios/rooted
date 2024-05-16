//
// Created by Jeffrey Zhou on 4/29/24.
//

#ifndef ROOTED_COLLECTEDROCKEVENT_H
#define ROOTED_COLLECTEDROCKEVENT_H

#include <cugl/cugl.h>

using namespace cugl::physics2::net;
using namespace cugl::net;
using namespace cugl;

class CollectedRockEvent : public NetEvent {

protected:
    NetcodeSerializer _serializer;
    NetcodeDeserializer _deserializer;

    std::string _uuid;
    int _rockID;

public:
    /**
    * This method is used by the NetEventController to create a new event of using a
    * reference of the same type.
    *
    * Not that this method is not static, it differs from the static alloc() method
    * and all methods must implement this method.
    */
    std::shared_ptr<NetEvent> newEvent() override;

    static std::shared_ptr<NetEvent> allocCollectedRockEvent(std::string _uuid, int rockid);

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

    std::string getUUID() { return _uuid; }

    int getRockID() { return _rockID; }
};


#endif //ROOTED_COLLECTEDROCKEVENT_H
