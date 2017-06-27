/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_matrix

#include "ecbuild/boost_test_framework.h"

#include "eckit/maths/Matrix.h"

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_matrix )

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_determinant )
{
  typedef eckit::maths::Matrix<double> Matrix;

  Matrix m(5,5);
  m(0,0) =  9.;  m(0,1) =  6.; m(0,2) =  2.; m(0,3) =  0.; m(0,4) =  3.;
  m(1,0) =  3.;  m(1,1) =  6.; m(1,2) =  8.; m(1,3) = 10.; m(1,4) = 12.;
  m(2,0) =  4.;  m(2,1) =  8.; m(2,2) =  2.; m(2,3) =  6.; m(2,4) =  9.;
  m(3,0) =  1.;  m(3,1) =  5.; m(3,2) =  5.; m(3,3) =  3.; m(3,4) =  2.;
  m(4,0) =  1.;  m(4,1) =  3.; m(4,2) =  6.; m(4,3) =  8.; m(4,4) = 10.;

  BOOST_CHECK_CLOSE( m.determinant(), 1124, 1.e-10 );
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( test_inverse )
{
  typedef eckit::maths::Matrix<double> Matrix;

  Matrix m(5,5);
  m(0,0) =  9.;  m(0,1) =  6.; m(0,2) =  2.; m(0,3) =  0.; m(0,4) =  3.;
  m(1,0) =  3.;  m(1,1) =  6.; m(1,2) =  8.; m(1,3) = 10.; m(1,4) = 12.;
  m(2,0) =  4.;  m(2,1) =  8.; m(2,2) =  2.; m(2,3) =  6.; m(2,4) =  9.;
  m(3,0) =  1.;  m(3,1) =  5.; m(3,2) =  5.; m(3,3) =  3.; m(3,4) =  2.;
  m(4,0) =  1.;  m(4,1) =  3.; m(4,2) =  6.; m(4,3) =  8.; m(4,4) = 10.;


  // Expected inverse
  Matrix minv(5,5);
  minv(0,0) = 0.03024911;  minv(0,1) =  0.78113879; minv(0,2) = -0.1405694; minv(0,3) = -0.29181495; minv(0,4) = -0.76156584;
  minv(1,0) = -0.01067616;  minv(1,1) = -0.33451957; minv(1,2) = 0.16725979; minv(1,3) = 0.22064057; minv(1,4) = 0.20996441;
  minv(2,0) = 0.12366548;  minv(2,1) = -0.54181495; minv(2,2) = -0.10409253; minv(2,3) = 0.27758007; minv(2,4) = 0.65124555;
  minv(3,0) = -0.31939502;  minv(3,1) = 2.07562278; minv(3,2) = -0.16281139; minv(3,3) = -0.5658363; minv(3,4) = -2.13523132;
  minv(4,0) = 0.18149466;  minv(4,1) = -1.31316726; minv(4,2) = 0.15658363; minv(4,3) = 0.24911032; minv(4,4) = 1.43060498;

  // Compute inverse
  Matrix mi(5,5);
  mi.noalias() = m.inverse();

  // Compute identity matrix
  Matrix I(5,5);
  I = m*mi;

  // Verify results
  for( int i=0; i<m.rows(); ++i) {
    for( int j=0; j<m.cols(); ++j) {
      BOOST_CHECK_CLOSE(mi(i,j),minv(i,j),1.e-4);
      if( i==j ) BOOST_CHECK_CLOSE( I(i,j),1.,1.e-8);
      else BOOST_CHECK_SMALL( I(i,j), 1.e-10 );
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
