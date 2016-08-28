/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#define BOOST_TEST_MODULE eckit_test_mpi
#include "ecbuild/boost_test_framework.h"

// --------------------------------------------------------------

#include "eckit/config/ResourceMgr.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/runtime/Main.h"
#include "eckit/mpi/mpi.h"
#include "eckit/log/Log.h"

#include "eckit/log/Log.h"

using namespace eckit;
using namespace eckit::mpi;

struct MPIFixture {

  MPIFixture()
  {
  }

};

BOOST_GLOBAL_FIXTURE( MPIFixture );

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_comm )
{
  // Log::info() << "mpi size = " << mpi::size() << std::endl;

  // if ( rank() == 0 )
  // {
  //   LocalPathName path("test_parallelcontext_resources.txt");
  //   std::ofstream file( path.c_str(), std::ios_base::out );
  //   file << "integer=1\n"
  //        << "string='hello world'\n";
  //   file.close();
  // }

  // ResourceMgr::appendConfig("test_parallelcontext_resources.txt");

  // BOOST_CHECK_EQUAL( (int) Resource<int>("integer",0) , 1 );
  // BOOST_CHECK_EQUAL( (std::string) Resource<std::string>("string","") , std::string("hello world") );

}

//-------------------------------------------------------------------------

