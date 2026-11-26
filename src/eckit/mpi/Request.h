// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_Request_h
#define eckit_mpi_Request_h

#include <iosfwd>

#include "eckit/memory/Counted.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class RequestContent : public Counted {
public:

    ~RequestContent() override;

    virtual void print(std::ostream&) const = 0;

    virtual int request() const = 0;

    virtual bool test() = 0;
};

//----------------------------------------------------------------------------------------------------------------------

/// Request by construction has always a valid content_
/// @invariant content_ is not null

class Request {

public:  // methods

    /// Null request constructor
    Request();
    /// Request constructor from the Request() integer
    /// Use only for interfacing with Fortran
    Request(int);
    /// Constructor
    Request(RequestContent*);

    ~Request();

    Request(const Request&);

    Request& operator=(const Request&);

    template <class T>
    T& as() {
        return dynamic_cast<T&>(*content_);
    }

    /// Returns this request interpreted as a int by the underlying implementation
    /// Use only for interfacing with Fortran
    int request() const;

    bool test();

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Request& o) {
        o.print(s);
        return s;
    }

private:  // members

    RequestContent* content_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
