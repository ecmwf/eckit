/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. In applying
 * this licence, ECMWF does not waive the privileges and immunities granted to it by virtue
 * of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

// source: https://stackoverflow.com/a/17890914/396967

#ifndef BOOST_AUTO_PARAM_H
#define BOOST_AUTO_PARAM_H

#include <boost/test/unit_test_suite.hpp>
#include <boost/test/parameterized_test.hpp>

#define BOOST_FIXTURE_PARAM_TEST_CASE( test_name, F, P, mbegin, mend )  \
struct test_name : public F                                             \
{                                                                       \
    typedef P param_t;                                                  \
    void test_method(const param_t &);                                  \
};                                                                      \
                                                                        \
void BOOST_AUTO_TC_INVOKER( test_name )(const test_name::param_t &param) \
{                                                                       \
    test_name t;                                                        \
    t.test_method(param);                                               \
}                                                                       \
                                                                        \
BOOST_AUTO_TU_REGISTRAR( test_name )(                                   \
    boost::unit_test::make_test_case(                                   \
       &BOOST_AUTO_TC_INVOKER( test_name ), #test_name,                 \
       (mbegin), (mend)));                                              \
                                                                        \
void test_name::test_method(const param_t &param)                       \

// *******

#define BOOST_AUTO_PARAM_TEST_CASE( test_name, param_type, mbegin, mend )           \
   BOOST_FIXTURE_PARAM_TEST_CASE( test_name,                            \
                                  BOOST_AUTO_TEST_CASE_FIXTURE,         \
                                  param_type,                           \
                                  mbegin, mend)

#endif // BOOST_AUTO_PARAM_H
