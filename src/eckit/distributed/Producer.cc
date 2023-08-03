/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/TimeStamp.h"

#include "eckit/distributed/Producer.h"
#include "eckit/distributed/Message.h"
#include "eckit/distributed/Transport.h"

using eckit::Log;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

Producer::Producer(Transport &transport):
    Actor(transport) {
}

void Producer::run() {

    transport_.initialise();
    eckit::Log::info()  << "starting " << std::endl;

    {
        eckit::Timer timer("Producing messages");
        Message msg;

        while (produce(msg)) {
            // eckit::Log::info()
            //             << " send message "
            //             << " tag " << msg.tag()
            //             << " size " << msg.messageSize()
            //             << std::endl;

            sendMessageToNextWorker(msg);
            // eckit::Log::info()  << " done message " << std::endl;

            msg.rewind();
        }
    }

    eckit::Log::info() << "all messages sent" << std::endl;

    // send finishing messages
    {
        eckit::Timer timer("Shuting down");
        sendShutDownMessage();
    }

    eckit::Log::info()  << "exiting " << std::endl;

    transport_.synchronise();

}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

