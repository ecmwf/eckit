/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/log/Log.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/log/ResourceUsage.h"

#include "eckit/distributed/Consumer.h"
#include "eckit/distributed/Message.h"
#include "eckit/distributed/Transport.h"

using eckit::Log;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

Consumer::Consumer(Transport &transport):
    Actor(transport) {
}

void Consumer::run() {

    // eckit::TraceResourceUsage<LibPGen> usage("Message::write()");
    // eckit::Log::info() << eckit::TimeStamp() << " " <<  transport_.title()  << " starting " << std::endl;

    Message msg;

    while (true) {

        msg.rewind();
        // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " begin getNextMessage" << std::endl;

        getNextMessage(msg);
        // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " end getNextMessage" << std::endl;

        if (msg.shutdownRequested()) {
            msg.rewind();
            shutdown(msg);
            sendStatisticsToProducer(msg);
            break;
        }

        try {
            // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " begin consume" << std::endl;

            consume(msg);
            // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " end consume" << std::endl;

        } catch (eckit::Exception &e) {
            eckit::Log::info() << "Failure: "  << e.what() << std::endl;
            failure(msg);
            // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " end failure" << std::endl;

        }
    }

    // eckit::Log::info() << eckit::TimeStamp() << " " << transport_.title()  << " exiting " << std::endl;
    transport_.synchronise();

}

void Consumer::shutdown(Message &message) {
    message << "OK";
}

void Consumer::failure(Message &message) {

}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

