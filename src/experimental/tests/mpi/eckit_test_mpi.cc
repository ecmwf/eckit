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

#include "eckit/mpi/Environment.h"
#include "eckit/mpi/Comm.h"

using namespace eckit;
using namespace eckit::mpi;

struct MPIFixture {
    MPIFixture()  { mpi::Environment::instance().initialize(); }
    ~MPIFixture() { mpi::Environment::instance().finalize(); }
};

BOOST_GLOBAL_FIXTURE( MPIFixture );

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_comm )
{
  BOOST_CHECK_EQUAL( mpi::Environment::instance().initialized() , true );
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_broadcast )
{
  size_t root = 0;

  int d[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  BOOST_TEST_CHECKPOINT("Test value");
  {
    float val;
    if( mpi::comm().rank() == root ) {
      val = 3.14f;
    }

    float* pval = &val;
    BOOST_CHECK_NO_THROW( mpi::comm().broadcast(val, root) );
    BOOST_CHECK_NO_THROW( mpi::comm().broadcast(pval, pval+1, root) );

    // check results
    BOOST_CHECK_CLOSE( val, 3.14f, 0.0001 );
  }

  BOOST_TEST_CHECKPOINT("Test vector");
  {
    std::vector<int> data(10);
    if(mpi::comm().rank() == root) {
      data.assign(d,d+10);
    }
    BOOST_CHECK_NO_THROW( mpi::comm().broadcast(data, root) );

    // check results
    BOOST_CHECK_EQUAL( data.size(), 10u );
    BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(),data.end(),d,d+10);
  }

  BOOST_TEST_CHECKPOINT("Test raw data");
  {
    std::vector<int> data(10);
    if( mpi::comm().rank() == root )
    {
      data.assign(d,d+10);
    }

    BOOST_CHECK_NO_THROW( mpi::comm().broadcast(data.begin(), data.end(), root) );

    // check results
    BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(), data.end(), d, d+10);
  }
}

BOOST_AUTO_TEST_CASE( test_all_reduce )
{
  int d = int(mpi::comm().rank()) + 1;
  std::pair<double,int> v(-d, mpi::comm().rank());
  std::vector<float> arr(5, mpi::comm().rank()+1);

  BOOST_TEST_CHECKPOINT("Testing all_reduce");
  {
    int sum;
    int prod;
    int max;
    int min;
    std::pair<double,int> maxloc;
    std::pair<double,int> minloc;

    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(d, sum,    mpi::sum())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(d, prod,   mpi::prod()) );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(d, max,    mpi::max())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(d, min,    mpi::min())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(v, maxloc, mpi::maxloc())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduce(v, minloc, mpi::minloc())   );

    // check results
    int s=0;
    int p=1;
    for( size_t j=0; j<mpi::comm().size(); ++j ) {
      s += (j+1);
      p *= (j+1);
    }
    BOOST_CHECK_EQUAL( sum, s );
    BOOST_CHECK_EQUAL( prod, p );
    BOOST_CHECK_EQUAL( min, 1 );
    BOOST_CHECK_EQUAL( size_t(max), mpi::comm().size() );
    BOOST_CHECK_EQUAL( minloc.first, -double(mpi::comm().size()) );
    BOOST_CHECK_EQUAL( size_t(minloc.second), mpi::comm().size()-1 );
    BOOST_CHECK_EQUAL( maxloc.first, -double(1) );
    BOOST_CHECK_EQUAL( maxloc.second, 0 );
  }

  BOOST_TEST_CHECKPOINT("Testing all_reduce inplace");
  {
    int sum = d;
    int prod = d;
    int max = d;
    int min = d;

    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(sum, mpi::sum())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(prod, mpi::prod()) );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(max, mpi::max())   );
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(min, mpi::min())   );

    // check results
    int s=0;
    int p=1;
    for( size_t j=0; j<mpi::comm().size(); ++j ) {
      s += (j+1);
      p *= (j+1);
    }
    BOOST_CHECK_EQUAL( sum, s );
    BOOST_CHECK_EQUAL( prod, p );
    BOOST_CHECK_EQUAL( min, 1 );
    BOOST_CHECK_EQUAL( size_t(max), mpi::comm().size() );

    std::vector<float> expected;

    expected = std::vector<float>(5, mpi::comm().size());
    std::vector<float> maxvec = arr;
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(maxvec.begin(), maxvec.end(), mpi::max()) );
    BOOST_CHECK_EQUAL_COLLECTIONS(maxvec.begin(),maxvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,1);
    std::vector<float> minvec = arr;
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(minvec.begin(),minvec.end(), mpi::min()) );
    BOOST_CHECK_EQUAL_COLLECTIONS(minvec.begin(),minvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,s);
    std::vector<float> sumvec = arr;
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(sumvec.begin(),sumvec.end(), mpi::sum()) );
    BOOST_CHECK_EQUAL_COLLECTIONS(sumvec.begin(),sumvec.end(),expected.begin(),expected.end());

    expected = std::vector<float>(5,p);
    std::vector<float> prodvec = arr;
    BOOST_CHECK_NO_THROW( mpi::comm().allReduceInPlace(prodvec.begin(),prodvec.end(), mpi::prod()) );
    BOOST_CHECK_EQUAL_COLLECTIONS(prodvec.begin(),prodvec.end(),expected.begin(),expected.end());
  }
}


BOOST_AUTO_TEST_CASE( test_all_to_all )
{
  std::vector< std::vector<int> > send(mpi::comm().size(), std::vector<int>(1, mpi::comm().rank()));
  std::vector< std::vector<int> > recv(mpi::comm().size());

  BOOST_CHECK_NO_THROW( mpi::comm().all_to_all( send, recv ) );

  // check results
  std::vector< std::vector<int> > expected(mpi::comm().size());
  for(size_t j=0; j<mpi::comm().size(); ++j)
    expected[j]=std::vector<int>(1,j);

  BOOST_CHECK_EQUAL(recv.size(), expected.size());
  for (size_t i = 0; i < mpi::comm().size(); ++i)
      BOOST_CHECK_EQUAL_COLLECTIONS(recv[i].begin(), recv[i].end(),
                                    expected[i].begin(), expected[i].end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___simple )
{
  int send = mpi::comm().rank();
  std::vector<int> recv(mpi::comm().size());

  BOOST_CHECK_NO_THROW( mpi::comm().allGather(send, recv.begin(), recv.end()) );

  // check results
  std::vector<int> expected(mpi::comm().size());
  for(size_t j=0; j<mpi::comm().size(); ++j)
    expected[j]=j;

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(),recv.end(),expected.begin(),expected.end());
}

//-------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_all_gather___buffer )
{
  std::vector<int> send(mpi::comm().rank(), mpi::comm().rank());
  mpi::Buffer<int> recv(mpi::comm().size());

  BOOST_CHECK_NO_THROW( mpi::comm().allGatherv(send.begin(), send.end(), recv) );

  // check results
  std::vector<int> expected;
  for(size_t j=0; j<mpi::comm().size(); ++j )
  {
    for( size_t i=0; i<j; ++i )
      expected.push_back(j);
  }

  BOOST_CHECK_EQUAL_COLLECTIONS(recv.begin(), recv.end(), expected.begin(), expected.end());
}

//-------------------------------------------------------------------------
