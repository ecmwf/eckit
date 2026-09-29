// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   Actor.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#pragma once

#include <string>


namespace eckit::distributed {

class Transport;
class Message;

//----------------------------------------------------------------------------------------------------------------------

class Actor {
public:

    enum MessageTags {
        READY,
        WORK,
        SHUTDOWN,
        OPEN,
        WRITE,
        CLOSE,
        STATISTICS,
        BYE
    };

public:  // methods

    Actor(Transport& transport);

    Actor(const Actor&)            = delete;
    Actor& operator=(const Actor&) = delete;
    Actor(Actor&&)                 = delete;
    Actor& operator=(Actor&&)      = delete;

    virtual ~Actor();

    virtual void run()      = 0;
    virtual void finalise() = 0;

    virtual void messageFromWorker(Message& message, int worker) const;
    virtual void messageFromWriter(Message& message, int worker) const;
    virtual void sendStatisticsToProducer(const Message& message) const;

    virtual void sendMessageToNextWorker(const Message& message) const;
    virtual void getNextWorkMessage(Message& message) const;
    virtual void getNextWriteMessage(Message& message) const;
    virtual void sendToWriter(int writer, const Message& message) const;

    virtual void sendShutDownMessage() const;

    static const char* tagName(int);

protected:  // members

    Transport& transport_;
    std::string title_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed
