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

BOOST_AUTO_TEST_CASE( test_broadcast )
{
  size_t root = 0;

  int d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  BOOST_CHECKPOINT("Test value");
  {
    float val;
    if( eckit::mpi::rank() == root )
    {
      val = 3.14;
    }
    BOOST_CHECK( mpi::broadcast(val,root) == MPI_SUCCESS );

    // check results
    BOOST_CHECK_CLOSE( val, 3.14, 0.0001 );
  }

  BOOST_CHECKPOINT("Test vector");
  {
    std::vector<int> data;
    if( eckit::mpi::rank() == root )
    {
      data.assign(d,d+10);
    }
    bool resize = true;
    BOOST_CHECK( mpi::broadcast(data,root,resize) == MPI_SUCCESS );

    // check results
    BOOST_CHECK_EQUAL( data.size(), 10 );
    BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(),data.end(),d,d+10);
  }

  BOOST_CHECKPOINT("Test raw data");
  {
    std::vector<int> data(10);
    if( eckit::mpi::rank() == root )
    {
      data.assign(d,d+10);
    }
    BOOST_CHECK( mpi::broadcast(data.data(),data.size(),root) == MPI_SUCCESS );

    // check results
    BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(),data.end(),d,d+10);
  }
}

BOOST_AUTO_TEST_CASE( test_all_reduce )
{
  int success;
  int d = eckit::mpi::rank()+1;
  std::pair<double,int> v(-d,eckit::mpi::rank());
  std::vector<float> arr(5,eckit::mpi::rank()+1);

  BOOST_CHECKPOINT("Testing all_reduce");
  {
    int sum;
    int prod;
    int max;
    int min;
    std::pair<double,int> maxloc;
    std::pair<double,int> minloc;

    success = mpi::all_reduce(d,sum,mpi::sum());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(d,prod,mpi::prod()); BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(d,max,mpi::max());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(d,min,mpi::min());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(v,maxloc,mpi::maxloc());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(v,minloc,mpi::minloc());   BOOST_CHECK( success == MPI_SUCCESS );

    // check results
    int s=0;
    int p=1;
    for( size_t j=0; j<eckit::mpi::size(); ++j ) {
      s += (j+1);
      p *= (j+1);
    }
    BOOST_CHECK_EQUAL( sum, s );
    BOOST_CHECK_EQUAL( prod, p );
    BOOST_CHECK_EQUAL( min, 1 );
    BOOST_CHECK_EQUAL( max, eckit::mpi::size() );
    BOOST_CHECK_EQUAL( minloc.first, -double(eckit::mpi::size()) );
    BOOST_CHECK_EQUAL( minloc.second, eckit::mpi::size()-1 );
    BOOST_CHECK_EQUAL( maxloc.first, -double(1) );
    BOOST_CHECK_EQUAL( maxloc.second, 0 );
  }

  BOOST_CHECKPOINT("Testing all_reduce inplace");
  {
    int sum = d;
    int prod = d;
    int max = d;
    int min = d;

    success = mpi::all_reduce(sum,mpi::sum());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(prod,mpi::prod()); BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(max,mpi::max());   BOOST_CHECK( success == MPI_SUCCESS );
    success = mpi::all_reduce(min,mpi::min());   BOOST_CHECK( success == MPI_SUCCESS );

    // check results
    int s=0;
    int p=1;
    for( size_t j=0; j<eckit::mpi::size(); ++j ) {
      s += (j+1);
      p *= (j+1);
    }
    BOOST_CHECK_EQUAL( sum, s );
    BOOST_CHECK_EQUAL( prod, p );
    BOOST_CHECK_EQUAL( min, 1 );
    BOOST_CHECK_EQUAL( max, eckit::mpi::size() );

    std::vector<float> expected;

    expected = std::vector<float>(5,eckit::mpi::size());
    std::vector<float> maxvec = arr;
    success = mpi::all_reduce(maxvec.data(),maxvec.size(),mpi::max()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(maxvec.begin(),maxvec.end(),expected.begin(),expected.end());
    maxvec = arr;
    success = mpi::all_reduce(maxvec,mpi::max()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(maxvec.begin(),maxvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,1);
    std::vector<float> minvec = arr;
    success = mpi::all_reduce(minvec.data(),minvec.size(),mpi::min()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(minvec.begin(),minvec.end(),expected.begin(),expected.end());
    minvec = arr;
    success = mpi::all_reduce(minvec,mpi::min()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(minvec.begin(),minvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,s);
    std::vector<float> sumvec = arr;
    success = mpi::all_reduce(sumvec.data(),sumvec.size(),mpi::sum()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(sumvec.begin(),sumvec.end(),expected.begin(),expected.end());
    sumvec = arr;
    success = mpi::all_reduce(sumvec,mpi::sum()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(sumvec.begin(),sumvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,p);
    std::vector<float> prodvec = arr;
    success = mpi::all_reduce(prodvec.data(),prodvec.size(),mpi::prod()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(prodvec.begin(),prodvec.end(),expected.begin(),expected.end());
    prodvec = arr;
    success = mpi::all_reduce(prodvec,mpi::prod()); BOOST_CHECK( success == MPI_SUCCESS );
    BOOST_CHECK_EQUAL_COLLECTIONS(prodvec.begin(),prodvec.end(),expected.begin(),expected.end());

  }



}


BOOST_AUTO_TEST_CASE( test_all_to_all )
{
  std::vector< std::vector<int> > send(mpi::size(), std::vector<int>(1,mpi::rank()));
  std::vector< std::vector<int> > recv(mpi::size());

  int success = all_to_all( mpi::Comm(), send, recv );

  BOOST_CHECK( success == MPI_SUCCESS );

  // check results
  std::vector< std::vector<int> > expected(mpi::size());
  for(size_t j=0; j<mpi::size(); ++j)
    expected[j]=std::vector<int>(1,j);

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___simple )
{
  int send = mpi::rank();
  std::vector<int> recv;

  int success = all_gather( mpi::Comm(), send, recv );

  BOOST_CHECK( success == MPI_SUCCESS );

  // check results
  std::vector<int> expected(mpi::size());
  for(size_t j=0; j<mpi::size(); ++j)
    expected[j]=j;

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___buffer )
{
  std::vector<int> send(mpi::rank(),mpi::rank());
  mpi::Buffer<int> recv;

  int success = all_gather( mpi::Comm(), send, recv );

  BOOST_CHECK( success == MPI_SUCCESS );

  // check results
  std::vector<int> expected;
  for(size_t j=0; j<mpi::size(); ++j )
  {
    for( size_t i=0; i<j; ++i )
      expected.push_back(j);
  }

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------


