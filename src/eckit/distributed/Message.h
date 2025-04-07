/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Message.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_Message_H
#define eckit_Message_H

#include <string>

#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

#include "eckit/distributed/Actor.h"

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

class Message : public eckit::Stream {

public:  // methods

    Message(int tag = Actor::WORK, size_t size = 1024 * 1024);
    ~Message();

    void rewind();
    bool shutdownRequested() const;

    void* messageData();
    const void* messageData() const;
    size_t bufferSize() const;
    size_t messageSize() const;
    void messageReceived(int, int);

    const void* getBlob(size_t& size);

    void reserve(size_t);

    int tag() const;
    int source() const;

    static const Message& readyMessage();
    static const Message& shutdownMessage();

    friend std::ostream& operator<<(std::ostream& s, const Message& x) {
        x.print(s);
        return s;
    }

private:  // members

    int tag_;
    int source_;

    eckit::Buffer buffer_;

    size_t position_;

    bool blob_;

private:  // methods

    // From Stream
    virtual std::string name() const;
    virtual long write(const void*, long);
    virtual long read(void*, long);

    void print(std::ostream& out) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif
