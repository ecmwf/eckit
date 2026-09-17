// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
