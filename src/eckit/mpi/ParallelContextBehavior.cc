/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/mpi/ParallelContextBehavior.h"
#include "eckit/mpi/mpi.h"
#include "eckit/mpi/BroadcastFile.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace mpi {

//-----------------------------------------------------------------------------

ParallelContextBehavior::ParallelContextBehavior() :
  ContextBehavior()
{
  mpi::Environment::instance();
}

struct ParallelScriptReadPolicy
{
  int root; // master task that reads

  /// @return true if file was read
  friend bool read_file(const ParallelScriptReadPolicy& policy, const PathName& path, config::Script& script )
  {
    std::stringstream stream;
    bool retval = mpi::broadcast_file(path,stream,policy.root);
    script.readStream(stream);
    return retval;
  }
};


config::Script::ReadPolicy ParallelContextBehavior::readScriptPolicy()
{
  ParallelScriptReadPolicy policy;
  policy.root = 0;
  return policy;
}

//-----------------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

