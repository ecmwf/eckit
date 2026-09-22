// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/distributed/Producer.h"

#include "eckit/distributed/Message.h"
#include "eckit/distributed/Transport.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/TimeStamp.h"

using eckit::Log;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

Producer::Producer(Transport& transport) : Actor(transport) {}

void Producer::run() {

    transport_.initialise();
    eckit::Log::info() << "starting " << std::endl;

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

    eckit::Log::info() << "exiting " << std::endl;

    transport_.synchronise();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed
