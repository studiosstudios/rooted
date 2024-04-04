//
//  UnrootEvent.h
//  Rooted
//
//  Created by Kimmy Lin on 3/19/24.
//

#ifndef UnrootEvent_h
#define UnrootEvent_h

#include <cugl/cugl.h>
using namespace cugl::physics2::net;
using namespace cugl::net;
using namespace cugl;

class UnrootEvent : public NetEvent {
protected:
    NetcodeSerializer _serializer;
    NetcodeDeserializer _deserializer;
    
    std::string _uuid;
    int _plantingspotid;
    
public:
    /**
    * This method is used by the NetEventController to create a new event of using a
    * reference of the same type.
    *
    * Not that this method is not static, it differs from the static alloc() method
    * and all methods must implement this method.
    */
   std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocUnrootEvent(std::string uuid, int plantingspotid);
    
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
    
    /** Gets the uuid of rooted carrot associated with the event. */
    std::string getUUID() { return _uuid; }
    
    /** Gets the uuid of planting spot associated with the event */
    int getPlantingSpotID() { return _plantingspotid; }
};

#endif /* UnrootEvent_h */
