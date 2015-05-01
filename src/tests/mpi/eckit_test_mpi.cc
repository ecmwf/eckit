/*
 * (C) Copyright 1996-2014 ECMWF.
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

// Teach boost how to write a std::vector<T>
namespace boost {

template <typename T>
inline boost::wrap_stringstream& operator<<(boost::wrap_stringstream& wrapped, std::vector<T> const& v)
{
  wrapped << '[';
  for (int j=0; j<v.size(); ++j) {
      wrapped << (j!=0 ? "," : "") << v[j];
  }
  return wrapped << ']';
}

}

// --------------------------------------------------------------

#include "eckit/mpi/mpi.h"
#include "eckit/mpi/Collectives.h"

using namespace eckit;
using namespace eckit::mpi;

struct MPIFixture {
    MPIFixture()  { eckit::mpi::init(); }
    ~MPIFixture() { eckit::mpi::finalize(); }
};

BOOST_GLOBAL_FIXTURE( MPIFixture )

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_comm )
{
  BOOST_CHECK_EQUAL( DefaultComm::instance() , mpi::Comm() );
  BOOST_CHECK_EQUAL( CommWorld::instance() , mpi::world() );
  BOOST_CHECK_EQUAL( mpi::Comm() , mpi::world() );
  BOOST_CHECK_EQUAL( mpi::size() , mpi::Comm().size() );
  BOOST_CHECK_EQUAL( mpi::rank() , mpi::Comm().rank() );
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_to_all )
{
  std::vector< std::vector<int> > send(mpi::size(), std::vector<int>(1,mpi::rank()));
  std::vector< std::vector<int> > recv(mpi::size());

  int success = all_to_all( mpi::Comm(), send, recv );

  // check results
  std::vector< std::vector<int> > expected(mpi::size());
  for( int j=0; j<mpi::size(); ++j )
    expected[j]=std::vector<int>(1,j);

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___simple )
{
  int send = mpi::rank();
  std::vector<int> recv;

  int success = all_gather( mpi::Comm(), send, recv );

  // check results
  std::vector<int> expected(mpi::size());
  for( int j=0; j<mpi::size(); ++j )
    expected[j]=j;

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___buffer )
{
  std::vector<int> send(mpi::rank(),mpi::rank());
  mpi::Buffer<int> recv;

  int success = all_gather( mpi::Comm(), send, recv );

  // check results
  std::vector<int> expected;
  for( int j=0; j<mpi::size(); ++j )
  {
    for( int i=0; i<j; ++i )
      expected.push_back(j);
  }

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------


