/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/distributed/Actor.h"
#include "eckit/distributed/Transport.h"

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

Actor::~Actor() {
}

Actor::Actor(Transport &transport):
    transport_(transport),
    title_(transport.title()) {
}

void Actor::sendMessageToNextWorker(const Message &msg) const {
    transport_.sendMessageToNextWorker(msg);
}

void Actor::sendStatisticsToProducer(const Message &msg) const {
    transport_.sendStatisticsToProducer(msg);
}

void Actor::getNextWorkMessage(Message &message) const {
    transport_.getNextWorkMessage(message);
}

void Actor::getNextWriteMessage(Message &message) const {
    transport_.getNextWriteMessage(message);
}

void Actor::sendToWriter(int writer, const Message &message) const {
    transport_.sendToWriter(writer, message);
}

void Actor::sendShutDownMessage() const {
    transport_.sendShutDownMessage(*this);
}

void Actor::messageFromWorker(Message&, int) const {
}

void Actor::messageFromWriter(Message&, int) const {
}

const char* Actor::tagName(int tag) {
    switch (tag) {

    case READY:
        return "READY";

    case WORK:
        return "WORK";

    case SHUTDOWN:
        return "SHUTDOWN";

    case OPEN:
        return "OPEN";

    case WRITE:
        return "WRITE";

    case CLOSE:
        return "CLOSE";

    case STATISTICS:
        return "STATISTICS";

    default:
        return "UNKNOWN";
    }
}
//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

