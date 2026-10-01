// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_SerialRequest_h
#define eckit_mpi_SerialRequest_h

#include "eckit/io/Buffer.h"
#include "eckit/mpi/DataType.h"
#include "eckit/mpi/Request.h"

namespace eckit::mpi {

class SerialRequestPool;
class Serial;

//----------------------------------------------------------------------------------------------------------------------

class SerialRequest : public RequestContent {

public:  // methods

    SerialRequest();

    ~SerialRequest() override;

    int request() const override;

    virtual int tag() const = 0;

    virtual bool isReceive() const = 0;

    bool test() override { return true; }

private:  // methods

    void print(std::ostream&) const override;

    bool handled() const { return handled_; }
    void handled(bool v) { handled_ = v; }

private:  // members

    friend class SerialRequestPool;
    friend class Serial;
    int request_;
    bool handled_{false};
};

//----------------------------------------------------------------------------------------------------------------------

class SendRequest : public SerialRequest {

public:  // methods

    SendRequest(const void* buffer, size_t count, Data::Code type, int tag);

    ~SendRequest() override;

    bool isReceive() const override { return false; }

    const void* buffer() const { return buffer_; }

    size_t count() const { return count_; }

    int tag() const override { return tag_; }

    Data::Code type() const { return type_; }

private:

    eckit::Buffer buffer_;
    size_t count_;
    int tag_;
    Data::Code type_;
};

//----------------------------------------------------------------------------------------------------------------------

class ReceiveRequest : public SerialRequest {

public:  // methods

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
    size_t count_;
    int tag_;
    Data::Code type_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
