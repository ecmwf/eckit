/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/FloatCompare.h"


using namespace std;
using namespace eckit;

namespace eckit_test {

class TestFloatCompare : public Tool {
public:

   TestFloatCompare(int argc,char **argv): Tool(argc,argv) {}

    ~TestFloatCompare() {}

    virtual void run();

    void test_large_numbers();
    void test_negative_large_numbers();
    void test_numbers_around_one();
    void test_numbers_around_negative_one();
    void test_numbers_between_one_and_zero();
    void test_numbers_between_minusone_and_zero();
    void test_comparisons_involving_zero();
    void test_comparisons_involving_infinity();
    void test_comparisons_involving_nan();
    void test_comparisons_opposite_side_of_zero();
    void test_comparisons_very_close_to_zero();

    bool is_equal(double a, double b, double epsilon) {
       return FloatCompare::is_equal(a,b,epsilon);
    }

    bool is_equal(double a, double b) {
       return FloatCompare::is_equal(a,b,0.00001);
    }
};

//-----------------------------------------------------------------------------

void TestFloatCompare::test_large_numbers()
{
   std::cout << "test_large_numbers\n";
   ASSERT(is_equal(1000000,1000000));
   ASSERT(is_equal(1000000,1000000.00001));
   ASSERT(is_equal(1000000.00001,1000000));

   ASSERT(!is_equal(1000000.0,1000001.0));
   ASSERT(!is_equal(1000001.0,1000000.0));

   // -----------------------------------------------
   ASSERT(is_equal(std::numeric_limits<double>::max(),std::numeric_limits<double>::max()));
   ASSERT(is_equal(std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::epsilon()));

   ASSERT(is_equal(std::numeric_limits<double>::min(),std::numeric_limits<double>::min()));
   ASSERT(is_equal(std::numeric_limits<double>::min(),std::numeric_limits<double>::min(),std::numeric_limits<double>::epsilon()));
}

void TestFloatCompare::test_negative_large_numbers()
{
   std::cout  << "test_negative_large_numbers " << std::numeric_limits<double>::min() << "\n";
   ASSERT(is_equal(-1000000, -1000000));
   ASSERT(is_equal(-1000000, -1000000.00001));
   ASSERT(is_equal(-1000000.00001,-1000000));

   ASSERT(!is_equal(-1000000.0, -1000001.0));
   ASSERT(!is_equal(-1000001.0, -1000000.0));

   // -----------------------------------------------
   ASSERT(is_equal(-std::numeric_limits<double>::max(),-std::numeric_limits<double>::max()));
   ASSERT(is_equal(-std::numeric_limits<double>::max(),-std::numeric_limits<double>::max(),std::numeric_limits<double>::epsilon()));

   ASSERT(is_equal(-std::numeric_limits<double>::min(),-std::numeric_limits<double>::min()));
   ASSERT(is_equal(-std::numeric_limits<double>::min(),-std::numeric_limits<double>::min(),std::numeric_limits<double>::epsilon()));
}

void TestFloatCompare::test_numbers_around_one()
{
   std::cout  << "test_numbers_around_one\n";
   ASSERT(is_equal(1.0000001, 1.0000002));
   ASSERT(is_equal(1.0000002, 1.0000001));

   ASSERT(is_equal(1.12345, 1.12346));
   ASSERT(is_equal(1.12345, 1.12344,0.001));

   ASSERT(!is_equal(1.0001, 1.0002));
   ASSERT(!is_equal(1.0002, 1.0001));
}

void TestFloatCompare::test_numbers_around_negative_one()
{
   std::cout  << "test_numbers_around_negative_one\n";
   ASSERT(is_equal(-1.0000001, -1.0000002));
   ASSERT(is_equal(-1.0000002, -1.0000001));

   ASSERT(!is_equal(-1.0001, -1.0002));
   ASSERT(!is_equal(-1.0002, -1.0001));
}

void TestFloatCompare::test_numbers_between_one_and_zero()
{
   std::cout  << "test_numbers_between_one_and_zero\n";
   ASSERT(is_equal(0.000000001000001, 0.000000001000002));
   ASSERT(is_equal(0.000000001000002, 0.000000001000001));

   ASSERT(!is_equal(0.00102, 0.00101));
   ASSERT(!is_equal(0.00101, 0.00102));

}

void TestFloatCompare::test_numbers_between_minusone_and_zero()
{
   std::cout  << "test_numbers_between_minusone_and_zero\n";
   ASSERT(is_equal(-0.000000001000001, -0.000000001000002));
   ASSERT(is_equal(-0.000000001000002, -0.000000001000001));

   ASSERT(!is_equal(-0.00102, -0.00101));
   ASSERT(!is_equal(-0.00101, -0.00102));
}

void TestFloatCompare::test_comparisons_involving_zero()
{
   std::cout  << "test_comparisons_involving_zero\n";
   ASSERT(is_equal(0.0, 0.0));
   ASSERT(is_equal(0.0, -0.0));
   ASSERT(is_equal(-0.0, -0.0));

   ASSERT(! is_equal(0.0001, 0.0));
   ASSERT(! is_equal(0.0, 0.0001));
   ASSERT(! is_equal(-0.0001, 0.0));
   ASSERT(! is_equal(0.0, -0.0001));

   ASSERT(is_equal(0.0, 1e-40, 0.01));
   ASSERT(is_equal(1e-40, 0.0, 0.01));
   ASSERT(! is_equal(1e-40, 0.0, 1e-41));
   ASSERT(! is_equal(0.0, 1e-40, 1e-41));

   ASSERT(is_equal(0.0, -1e-40, 0.1));
   ASSERT(is_equal(-1e-40, 0.0, 0.1));
   ASSERT(! is_equal(-1e-40, 0.0, 1e-41));
   ASSERT(! is_equal(0.0, -1e-40, 1e-41));
}

void TestFloatCompare::test_comparisons_involving_infinity()
{
   std::cout  << "test_comparisons_involving_infinity\n";

   if (std::numeric_limits<double>::has_infinity) {

      ASSERT(is_equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
      ASSERT(is_equal(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()));
   }
   else {
      std::cout  << "test_comparisons_involving_infinity NOT VALID on this platform\n";
   }
}

void TestFloatCompare::test_comparisons_involving_nan()
{
//    std::cout  << "test_comparisons_involving_nan\n";
//
//    // The value NaN (Not a Number) is used to represent a value that does not represent a real number.
//    // NaN's are represented by a bit pattern with an exponent of all 1s and a non-zero fraction. T
//    // there are two categories of NaN: QNaN (Quiet NaN) and SNaN (Signalling NaN).
//    //
//    // A QNaN is a NaN with the most significant fraction bit set.
//    // QNaN's propagate freely through most arithmetic operations.
//    // These values pop out of an operation when the result is not mathematically defined.
//
//    // An SNaN is a NaN with the most significant fraction bit clear.
//    // It is used to signal an exception when used in operations.
//    // SNaN's can be handy to assign to uninitialized variables to trap premature usage.
//
//    // Semantically, QNaN's denote indeterminate operations, while SNaN's denote invalid operations.
//
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), 0.0));
//    ASSERT(! is_equal(-0.0, std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), -0.0));
//    ASSERT(! is_equal(0.0, std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()));
//    ASSERT(! is_equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::max()));
//    ASSERT(! is_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::max()));
//    ASSERT(! is_equal(-std::numeric_limits<double>::max(), std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::min()));
//    ASSERT(! is_equal(std::numeric_limits<double>::min(), std::numeric_limits<double>::quiet_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::quiet_NaN(), -std::numeric_limits<double>::min()));
//    ASSERT(! is_equal(-std::numeric_limits<double>::min(), std::numeric_limits<double>::quiet_NaN()));
//
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), 0.0));
//    ASSERT(! is_equal(-0.0, std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), -0.0));
//    ASSERT(! is_equal(0.0, std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), std::numeric_limits<double>::infinity()));
//    ASSERT(! is_equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), std::numeric_limits<double>::max()));
//    ASSERT(! is_equal(std::numeric_limits<double>::max(), std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), -std::numeric_limits<double>::max()));
//    ASSERT(! is_equal(-std::numeric_limits<double>::max(), std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), std::numeric_limits<double>::min()));
//    ASSERT(! is_equal(std::numeric_limits<double>::min(), std::numeric_limits<double>::signaling_NaN()));
//    ASSERT(! is_equal(std::numeric_limits<double>::signaling_NaN(), -std::numeric_limits<double>::min()));
//    ASSERT(! is_equal(-std::numeric_limits<double>::min(), std::numeric_limits<double>::signaling_NaN()));
}

void TestFloatCompare::test_comparisons_opposite_side_of_zero()
{
   std::cout  << "test_comparisons_opposite_side_of_zero\n";

   ASSERT(! is_equal(1.000000001, -1.0));
   ASSERT(! is_equal(-1.0, 1.000000001));
   ASSERT(! is_equal(-1.000000001, 1.0));
   ASSERT(! is_equal(1.0, -1.000000001));

   ASSERT(is_equal(10.0 * std::numeric_limits<double>::min(), 10.0 * -std::numeric_limits<double>::min()));
   ASSERT(is_equal(10000 * std::numeric_limits<double>::min(), 10000 * -std::numeric_limits<double>::min()));
}

void TestFloatCompare::test_comparisons_very_close_to_zero()
{
   std::cout  << "test_comparisons_very_close_to_zero\n";

   ASSERT(is_equal(std::numeric_limits<double>::min(), -std::numeric_limits<double>::min()));
   ASSERT(is_equal(-std::numeric_limits<double>::min(), std::numeric_limits<double>::min()));
   ASSERT(is_equal(std::numeric_limits<double>::min(), 0));
   ASSERT(is_equal(0, std::numeric_limits<double>::min()));
   ASSERT(is_equal(-std::numeric_limits<double>::min(), 0));
   ASSERT(is_equal(0, -std::numeric_limits<double>::min()));


   ASSERT( is_equal(0.000000001, -std::numeric_limits<double>::min()));
   ASSERT( is_equal(0.000000001, std::numeric_limits<double>::min()));
   ASSERT( is_equal(std::numeric_limits<double>::min(), 0.000000001));
   ASSERT( is_equal(-std::numeric_limits<double>::min(), 0.000000001));


   ASSERT(! is_equal(0.000000001, -std::numeric_limits<double>::min(),  1e-10));
   ASSERT(! is_equal(0.000000001, std::numeric_limits<double>::min(),   1e-10));
   ASSERT(! is_equal(std::numeric_limits<double>::min(), 0.000000001,   1e-10));
   ASSERT(! is_equal(-std::numeric_limits<double>::min(), 0.000000001,  1e-10));
}

//-----------------------------------------------------------------------------

void TestFloatCompare::run()
{
   test_large_numbers();
   test_negative_large_numbers();
   test_numbers_around_one();
   test_numbers_around_negative_one();
   test_numbers_between_one_and_zero();
   test_numbers_between_minusone_and_zero();
   test_comparisons_involving_zero();
   test_comparisons_involving_infinity();
   test_comparisons_involving_nan();
   test_comparisons_opposite_side_of_zero();
   test_comparisons_very_close_to_zero();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestFloatCompare mytest(argc,argv);
    mytest.start();
    return 0;
}
