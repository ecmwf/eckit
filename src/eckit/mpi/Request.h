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

#include <vector>

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Request : public eckit::Owned {

public: // types

    typedef eckit::SharedPtr<Request> Ptr;
    typedef std::vector<Request::Ptr> Vector;

public: // methods

    virtual ~Request();

private: // methods

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Request& o) {
        o.print(s); return s;
    }

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
