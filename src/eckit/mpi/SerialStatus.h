/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_SerialStatus_h
#define eckit_mpi_SerialStatus_h

#include <iosfwd>

#include "eckit/mpi/Status.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class Serial;

class SerialStatus : public StatusContent {
    SerialStatus();

private:  // methods

    friend class Serial;

    int source() const override { return source_; }
    int tag() const override { return tag_; }
    int error() const override { return error_; }

    void print(std::ostream&) const override;

private:  // members

    int source_;
    int tag_;
    int error_;

    size_t count_;  ///< counts number of elements transfered in comm
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
