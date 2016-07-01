/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <utility>
#include "eckit/mpi/mpi.h"
#include "eckit/mpi/Exceptions.h"
#include "eckit/runtime/Context.h"

namespace eckit {
namespace mpi {

template<> MPI_Datatype datatype<char>()          { return MPI_CHAR; }
template<> MPI_Datatype datatype<int>()           { return MPI_INT; }
template<> MPI_Datatype datatype<unsigned int>()  { return MPI_UNSIGNED; }
template<> MPI_Datatype datatype<long>()          { return MPI_LONG; }
template<> MPI_Datatype datatype<unsigned long>() { return MPI_UNSIGNED_LONG; }
template<> MPI_Datatype datatype<float>()         { return MPI_FLOAT; }
template<> MPI_Datatype datatype<double>()        { return MPI_DOUBLE; }

template<> MPI_Datatype datatype<char>(char&)                   { return datatype<char>(); }
template<> MPI_Datatype datatype<int>(int&)                     { return datatype<int>(); }
template<> MPI_Datatype datatype<unsigned int>(unsigned int&)   { return datatype<unsigned int>(); }
template<> MPI_Datatype datatype<long>(long&)                   { return datatype<long>(); }
template<> MPI_Datatype datatype<unsigned long>(unsigned long&) { return datatype<unsigned long>(); }
template<> MPI_Datatype datatype<float>(float&)                 { return datatype<float>(); }
template<> MPI_Datatype datatype<double>(double&)               { return datatype<double>(); }

template<> MPI_Datatype datatype< std::pair<float,int> >()       { return MPI_FLOAT_INT; }
template<> MPI_Datatype datatype< std::pair<long,int> >()        { return MPI_LONG_INT; }
template<> MPI_Datatype datatype< std::pair<double,int> >()      { return MPI_DOUBLE_INT; }
template<> MPI_Datatype datatype< std::pair<short,int> >()       { return MPI_SHORT_INT; }
template<> MPI_Datatype datatype< std::pair<int,int> >()         { return MPI_2INT; }
template<> MPI_Datatype datatype< std::pair<long double,int> >() { return MPI_LONG_DOUBLE_INT; }

template<> MPI_Datatype datatype<float,int>()       { return MPI_FLOAT_INT; }
template<> MPI_Datatype datatype<long,int>()        { return MPI_LONG_INT; }
template<> MPI_Datatype datatype<double,int>()      { return MPI_DOUBLE_INT; }
template<> MPI_Datatype datatype<short,int>()       { return MPI_SHORT_INT; }
template<> MPI_Datatype datatype<int,int>()         { return MPI_2INT; }
template<> MPI_Datatype datatype<long double,int>() { return MPI_LONG_DOUBLE_INT; }

Environment::Environment()
{
  eckit::mpi::init( Context::instance().argc(), Context::instance().argvs() );
}

Environment::~Environment()
{
  eckit::mpi::finalize();
}

Environment& Environment::instance()
{
  static Environment env;
  return env;
}

bool initialized()
{
  int initialized;
  ECKIT_MPI_CHECK_RESULT( MPI_Initialized( &initialized ) );
  return initialized;
}

bool finalized()
{
  int finalized;
  ECKIT_MPI_CHECK_RESULT( MPI_Finalized( &finalized ) );
  return finalized;
}

void init(int argc, char *argv[])
{
  if( !initialized() )
    ECKIT_MPI_CHECK_RESULT( MPI_Init(&argc,&argv) );
}

void finalize()
{
  if( !finalized() )
    ECKIT_MPI_CHECK_RESULT( MPI_Finalize() );
}


size_t rank()
{
  return comm().rank();
}

size_t size()
{
  return comm().size();
}

void barrier()
{
   comm().barrier();
}

void abort()
{
   comm().abort();
}

Comm& comm()
{
  return DefaultComm::instance();
}

const Comm& world()
{
  return CommWorld::instance();
}

} // namespace mpi
} // namepsace eckit
