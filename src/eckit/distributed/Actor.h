/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Actor.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#pragma once

#include <string>

#include "eckit/memory/NonCopyable.h"


namespace eckit::distributed {

class Transport;
class Message;

//----------------------------------------------------------------------------------------------------------------------

class Actor : private eckit::NonCopyable {
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
