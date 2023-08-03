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
#include "eckit/log/Seconds.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/os/AutoAlarm.h"

#include "eckit/distributed/NoTransport.h"
#include "eckit/distributed/Transport.h"
#include "eckit/distributed/Message.h"
#include "eckit/distributed/Actor.h"

using eckit::Log;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

NoTransport::~NoTransport() {
}

NoTransport::NoTransport(const eckit::option::CmdArgs &args):
    Transport(args) {

}

void NoTransport::initialise() {

}

void NoTransport::abort() {
    eckit::Log::info() << "NoTransport::abort() called" << std::endl;
    ::exit(1);
}


void NoTransport::synchronise() {
}


bool NoTransport::single() const {
    return true;
}

bool NoTransport::writer() const {
    return false;
}

void NoTransport::sendMessageToNextWorker(const Message &msg) {
    NOTIMP;
}

void NoTransport::getNextWorkMessage(Message &message) {
    NOTIMP;
}

bool NoTransport::producer() const {
    NOTIMP;
}

void NoTransport::print(std::ostream& out) const {
    out << "NoTransport[]";
}

void NoTransport::sendToWriter(size_t writer, const Message &message) {
    NOTIMP;
}

void NoTransport::getNextWriteMessage(Message &message) {
    NOTIMP;
}


void NoTransport::sendStatisticsToProducer(const Message &message) {
    NOTIMP;
}

void NoTransport::sendShutDownMessage(const Actor&) {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

static TransportBuilder<NoTransport> builder("none");


} // namespace eckit

