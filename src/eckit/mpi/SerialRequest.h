/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_SerialRequest_h
#define eckit_mpi_SerialRequest_h

#include "eckit/mpi/Request.h"
#include "eckit/mpi/DataType.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Serial;

class SerialRequest : public RequestContent {

public: // methods

    SerialRequest();

private: // methods

    virtual void print(std::ostream&) const;

private: // members

    friend class Serial;

    void* recvbuf_;
    void* sendbuf_;
    size_t count_;
    int tag_;
    Data::Code type_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
