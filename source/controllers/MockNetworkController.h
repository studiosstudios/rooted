//
// Created by Jeffrey Zhou on 4/25/24.
//
// This is a lightweight mock network controller for the tutorial scene only.

#ifndef ROOTED_MOCKNETWORKCONTROLLER_H
#define ROOTED_MOCKNETWORKCONTROLLER_H

#include "NetworkController.h"

class MockNetworkController : public NetworkController {
private:
    std::queue<std::shared_ptr<NetEvent>> _eventQueue;
    bool _isHost;

public:

    static std::shared_ptr<MockNetworkController> alloc() {
        auto res = std::make_shared<MockNetworkController>();
        return (res->init() ? res : nullptr);
    }

    bool init();

    void setHost(bool host) { _isHost = host; }

    virtual bool isHost() const override { return _isHost; }

    virtual bool isInAvailable() override { return !_eventQueue.empty(); }

    virtual void pushOutEvent(const std::shared_ptr<NetEvent> &e) override;

    virtual std::shared_ptr<NetEvent> popInEvent() override;
};


#endif //ROOTED_MOCKNETWORKCONTROLLER_H
