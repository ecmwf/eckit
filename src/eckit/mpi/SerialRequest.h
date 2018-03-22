/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_SerialRequest_h
#define eckit_mpi_SerialRequest_h

#include "eckit/io/Buffer.h"
#include "eckit/mpi/Request.h"
#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

  class SerialRequestPool;

//----------------------------------------------------------------------------------------------------------------------

class SerialRequest : public RequestContent {

public: // methods

    SerialRequest();

    virtual ~SerialRequest();

    virtual int request() const;

    virtual int tag() const = 0;

    virtual bool isReceive() const = 0;

    virtual bool test() { return true; }

private: // methods

    virtual void print(std::ostream&) const;

private: // members

    friend class SerialRequestPool;
    int request_;
};

//----------------------------------------------------------------------------------------------------------------------

class SendRequest : public SerialRequest {

public: // methods

    SendRequest(const void* buffer, size_t count, Data::Code type, int tag);

    virtual ~SendRequest();

    virtual bool isReceive() const { return false; }

    const void* buffer() const { return buffer_; }

    size_t count() const { return count_; }

    virtual int tag() const { return tag_; }

    Data::Code type() const { return type_; }

private:

    eckit::Buffer buffer_;
    size_t count_;
    int tag_;
    Data::Code type_;
};

//----------------------------------------------------------------------------------------------------------------------

class ReceiveRequest : public SerialRequest {

public: // methods

    ReceiveRequest(void* buffer, size_t count, Data::Code type, int tag);

    virtual bool isReceive() const { return true; }

    /// Non const access is needed
    void* buffer() { return buffer_; }

    const void* buffer() const { return buffer_; }

    size_t count() const { return count_; }

    virtual int tag() const { return tag_; }

    Data::Code type() const { return type_; }

private:

    void* buffer_;
    size_t size_;
    size_t count_;
    int tag_;
    Data::Code type_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
