/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_ParallelContextBehavior_h
#define eckit_mpi_ParallelContextBehavior_h

#include <string>
#include "eckit/runtime/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace mpi {

//-----------------------------------------------------------------------------

class ParallelContextBehavior : public ContextBehavior {
public:
    ParallelContextBehavior();
    virtual ~ParallelContextBehavior() {}

    virtual std::string readConfig(const std::string& path);

private:
    int master_task_;
};


//-----------------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

#endif // eckit_mpi_ParallelContextBehavior_h
