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

//-----------------------------------------------------------------------------

namespace eckit {
namespace mpi {

//-----------------------------------------------------------------------------

ParallelContextBehavior::ParallelContextBehavior() :
  ContextBehavior(),
  master_task_(0)
{
  mpi::Environment::instance();
}


std::string ParallelContextBehavior::readConfig(const std::string& path)
{
  std::string config;

  /// This function lets only one MPI task read the config file,
  /// and broadcasts to remaining tasks. This is beneficial at
  /// large scale.
  std::stringstream stream;
  char* buf;
  int buf_len(0);
  if( eckit::mpi::rank() == master_task_ )
  {
    PathName p(path);
    if( p.exists() )
    {
      std::ifstream in;
      in.open ( p.asString().c_str() );
      if (!in)
        throw CantOpenFile( p.asString() );

      stream << in.rdbuf();
      in.close();
      std::string str = stream.str();
      buf = const_cast<char*>(str.c_str());
      buf_len = str.size();
      MPI_Bcast(&buf_len,1,eckit::mpi::datatype<int >(),master_task_,eckit::mpi::comm());
      if (buf_len)
        MPI_Bcast(buf,buf_len,eckit::mpi::datatype<char>(),master_task_,eckit::mpi::comm());
    }
    else
    {
      MPI_Bcast(&buf_len,1,eckit::mpi::datatype<int >(),master_task_,eckit::mpi::comm());
    }
  }
  else
  {
    MPI_Bcast(&buf_len,1,eckit::mpi::datatype<int>(),master_task_,eckit::mpi::comm());
    if( buf_len )
    {
      buf = new char[buf_len];
      MPI_Bcast(buf,buf_len,eckit::mpi::datatype<char>(),master_task_,eckit::mpi::comm());
      stream.write(buf,buf_len);
      delete[] buf;
    }
  }
  if (buf_len)
  {
    config = stream.str();
  }
  return config;
}

//-----------------------------------------------------------------------------

} // namespace mpi
} // namespace eckit

