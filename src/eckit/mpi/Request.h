/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Request_h
#define eckit_mpi_Request_h

#include <iosfwd>

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class RequestContent;

class Request {

public: // methods

    Request();

    ~Request();

private: // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Request& o) {
        o.print(s); return s;
    }

private: // members

    RequestContent* pimpl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
