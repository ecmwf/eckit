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
#include "eckit/config/ResourceFromFiles.h"
#include "eckit/mpi/ParallelContextBehavior.h"
#include "eckit/mpi/mpi.h"
#include "eckit/mpi/BroadcastFile.h"

namespace eckit {
namespace mpi {

//-----------------------------------------------------------------------------

struct ParallelReadPolicy
{
  int root; // master task that reads

  /// @return true if file was read
  friend bool read(const ParallelReadPolicy& policy, const PathName& path, std::stringstream& stream )
  {
    return mpi::broadcast_file(path,stream,policy.root);
  }

};

//-----------------------------------------------------------------------------

ParallelContextBehavior::ParallelContextBehavior() :
  StandardBehavior()
{
  mpi::Environment::instance();
}

FileReadPolicy ParallelContextBehavior::fileReadPolicy()
{
  ParallelReadPolicy mypolicy;
  mypolicy.root = 0;
  return mypolicy;
}

//-----------------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

