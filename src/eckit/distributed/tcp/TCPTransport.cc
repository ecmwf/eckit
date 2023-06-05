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
#include "eckit/log/Seconds.h"
#include "eckit/log/Plural.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Main.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/os/AutoAlarm.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/io/Select.h"

#include "eckit/distributed/tcp/TCPTransport.h"
#include "eckit/distributed/Transport.h"
#include "eckit/distributed/Message.h"
#include "eckit/distributed/Actor.h"

using namespace eckit;
using namespace eckit::net;

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

class Connection : public InstantTCPStream {
    Select& select_;
    TCPSocket socket_;
    size_t id_;
    bool active_;

private:

    TCPSocket& socket() { return socket_; }

public:
    Connection(Select& select, TCPSocket &socket, size_t id = 0):
        InstantTCPStream(socket),
        select_(select),
        socket_(socket),
        id_(id),
        active_(true) {
        select_.add(socket_);
    }

    ~Connection() {
        if (active_) {
            disconnect();
        }
    }

    size_t id() const { return id_; }

    bool active() const { return active_; }

    void disconnect() {
        active_ = false;
        select_.remove(socket_);
        socket_.close();
    }

    bool ready() {
        return active_ && select_.set(socket_);
    }

    std::string remoteHost() {
        return socket_.remoteHost();
    }

    int remotePort() {
        return socket_.remotePort();
    }
};

TCPTransport::TCPTransport(const option::CmdArgs &args):
    Transport(args),
    nextId_(0),
    master_(false),
    worker_(false),
    writer_(false) {


    size_t port = 7777;
    args.get("port", port);

    std::string hostname = Main::hostname();

    std::ostringstream oss;

    std::string host("localhost");
    if (args.get("host", host)) {
        // We are a consumer
        TCPClient client;
        producer_.reset(new Connection(select_, client.connect(host, port, 10, 60)));
        oss << "Consumer-" << ::getpid() << "@" << hostname;

    } else {
        // We are the producer
        accept_.reset(new TCPServer(port));
        select_.add(*accept_);
        oss << "Producer-" << ::getpid() << "@" << hostname;

    }

    title_ = oss.str();

    std::ostringstream oid;
    oid << hostname << "@" << ::getpid();
    id_ = oid.str();

}


TCPTransport::~TCPTransport() {
    for (auto j = connections_.begin(); j != connections_.end(); ++j) {
        delete (*j);
    }
}

bool TCPTransport::single() const {
    return false;
}

void TCPTransport::initialise() {
    accept();
}

void TCPTransport::synchronise() {
}

void TCPTransport::sendMessageToNextWorker(const Message &message) {
    while (!send(message)) {
        cleanup();
        Log::info() << TimeStamp()
                  << " "
                  << title()
                  << ", resending..."
                  << std::endl;
        if (connections_.empty()) {
            throw  SeriousBug("TCPTransport: no more workers");
        }
    }
}


bool TCPTransport::send(const Message &message) {

    cleanup();

    bool more = true;
    while (more) {
        more = false;

        while (!select_.ready(30)) {
            Log::info() <<  TimeStamp()
                      << " "
                      << title()
                      << ", waiting... "
                      << Plural(connections_.size(), "worker")
                      << " still active"
                      << std::endl;
        }

        if (select_.set(*accept_)) {
            accept();
            more = true;
        }
    }

    for (auto j = connections_.rbegin(); j != connections_.rend(); ++j) {
        Connection &connection = **j;
        if (connection.ready()) {
            try {

                size_t tag;
                connection >> tag;

                ASSERT(tag == Actor::READY);

                // Log::info() << TimeStamp()
                //           << " "
                //           << title()
                //           << " sending to worker "
                //           << connection.id()
                //           << " tag " << message.tag()
                //           << std::endl;

                connection << size_t(message.tag());
                connection << message.messageSize();
                connection.writeBlob(message.messageData(), message.messageSize());

                // Next time, we consider other connections first
                std::swap(*j, connections_[0]);

                return true;

            } catch (std::exception &e) {
                disconnect(e, connection);
                continue;
            }
        }
    }
    return false;
}


void TCPTransport::cleanup() {
    bool more = true;
    while (more) {
        more = false;
        for (auto j = connections_.begin(); j != connections_.end(); ++j) {
            Connection *connection = *j;
            if (!connection->active()) {
                delete connection;
                connections_.erase(j);
                more = true;
                break;
            }
        }
    }
}

Connection& TCPTransport::producerConnection() const {
    ASSERT(producer_);
    return *producer_;
}


void TCPTransport::disconnect() const {
// Close connection to producer
    ASSERT(producer_);
    producer_.reset(nullptr);
}


void TCPTransport::getNextWorkMessage(Message &message) {

    auto& connection = producerConnection();

    Log::info() << TimeStamp()
              << " "
              << title()
              << " TCPTransport::getNextWorkMessage -> send"
              << std::endl;

    connection << size_t(Actor::READY);

    size_t tag;
    connection >> tag;
    Log::info() << TimeStamp()
              << " "
              << title()
              << " TCPTransport::getNextWorkMessage got reply"
              << std::endl;

    size_t size;

    switch (tag) {

    case Actor::WORK:
        connection >> size;
        ASSERT(size <= message.bufferSize());
        connection.readBlob(message.messageData(), size);
        break;

    case Actor::SHUTDOWN:
        break;

    default:
        ASSERT(tag == Actor::WORK || tag == Actor::SHUTDOWN);
        break;
    }

    message.rewind();
    message.messageReceived(tag, connection.id());

}

void TCPTransport::sendStatisticsToProducer(const Message &message) {

    auto& connection = producerConnection();

    connection << size_t(Actor::STATISTICS);
    connection << message.messageSize();
    connection.writeBlob(message.messageData(), message.messageSize());


    // Close connection to producer
    // TODO: this is the wrong place

    disconnect();


}

bool TCPTransport::producer() const {
    return accept_ != 0;
}

void TCPTransport::accept()  {
    ASSERT(accept_);
    Log::info() << TimeStamp()
              << " "
              << title()
              << ", waiting for a connection"
              <<  std::endl;

    TCPSocket incoming(accept_->accept());

    Connection *connection = new Connection(select_, incoming, ++nextId_);
    connections_.push_back(connection);

    Log::info() << TimeStamp()
              << " "
              << title()
              << ", got connection from "
              << connection->remoteHost()
              << ":"
              <<  connection->remotePort()
              << ", worker: "
              << connection->id()
              << std::endl;
}

void TCPTransport::print(std::ostream &out) const {
    out << "TCPTransport[]";
}


void TCPTransport::abort() {
    cleanup();
    for (auto j = connections_.begin(); j != connections_.end(); ++j) {
        disconnect(**j);
        delete (*j);
    }
    connections_.clear();
}

bool TCPTransport::writer() const {
    return false;
}

void TCPTransport::sendToWriter(size_t writer, const Message &message) {
    NOTIMP;
}

void TCPTransport::getNextWriteMessage(Message &message) {
    NOTIMP;
}

void TCPTransport::disconnect(Connection& connection) const {
    Log::error()
            << TimeStamp()
            << " "
            << title()
            << " disconnect "
            << connection.id()
            << std::endl;
    connection.disconnect();
}

void TCPTransport::disconnect(std::exception& e, Connection& connection) const {

    Log::error()
            << TimeStamp()
            << " "
            << title()
            << " "
            << e.what()
            << std::endl;

    Log::error()
            << TimeStamp()
            << " "
            << title()
            << ", lost connection with worker "
            << connection.id()
            << std::endl;

    disconnect(connection);

}


void TCPTransport::sendShutDownMessage(const Actor& actor) {

    select_.remove(*accept_);

    while (connections_.size()) {

        bool more = true;
        while (more) {
            more = false;

            while (!select_.ready(30)) {
                Log::info() <<  TimeStamp()
                          << " "
                          << title()
                          << ", waiting... "
                          << Plural(connections_.size(), "worker")
                          << " still active"
                          << std::endl;
            }

            // if (select_.set(*accept_)) {
            //     accept();
            //     more = true;
            // }
        }

        Log::info() << TimeStamp()
                  << " "
                  << title()
                  << " "
                  << Plural(connections_.size(), "worker")
                  << " remaining"
                  << std::endl;


        for (auto j = connections_.begin(); j != connections_.end(); ++j) {
            Connection &connection = **j;
            if (connection.ready()) {
                try {

                    Message message;

                    size_t tag;
                    size_t size;

                    connection >> tag;

                    switch (tag) {

                    case Actor::READY:
                        Log::info() << TimeStamp()
                                  << " "
                                  << title()
                                  << " shutdown worker "
                                  << connection.id()
                                  << std::endl;
                        connection << size_t(Actor::SHUTDOWN);
                        break;

                    case Actor::STATISTICS:
                        connection >> size;

                        message.reserve(size);
                        connection.readBlob(message.messageData(), size);
                        actor.messageFromWorker(message, connection.id());
                        disconnect(connection);
                        break;

                    default:
                        ASSERT(tag == Actor::READY || tag == Actor::STATISTICS);
                        break;
                    }

                } catch (std::exception &e) {
                    disconnect(e, connection);
                }
            }
        }

        cleanup();
    }
}
//----------------------------------------------------------------------------------------------------------------------

static TransportBuilder<TCPTransport> builder("tcp");


} // namespace eckit

