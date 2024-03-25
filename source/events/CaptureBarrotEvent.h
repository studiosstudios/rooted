//
//  CaptureBarrotEvent.h
//  Rooted
//
//  Created by Kimmy Lin on 3/22/24.
//

#ifndef CaptureBarrotEvent_h
#define CaptureBarrotEvent_h

#include <cugl/cugl.h>
using namespace cugl::physics2::net;
using namespace cugl;
using namespace cugl::net;

class CaptureBarrotEvent : public NetEvent {
    
protected:
    NetcodeSerializer _serializer;
    NetcodeDeserializer _deserializer;
    
    std::string _carrotuuid;
    int _barrotid;
    
public:
    /**
    * This method is used by the NetEventController to create a new event of using a
    * reference of the same type.
    *
    * Not that this method is not static, it differs from the static alloc() method
    * and all methods must implement this method.
    */
   std::shared_ptr<NetEvent> newEvent() override;
    
    static std::shared_ptr<NetEvent> allocCaptureBarrotEvent(std::string carrotuuid, int barrotid);
    
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
    
    /** Gets the uuid of carrot associated with the event. */
    std::string getCarrotUUID() { return _carrotuuid; }
    
    /** Gets the uuid of barrot associated with the event **/
    int getBarrotID() { return _barrotid; }
};

#endif /* CaptureBarrot_h */