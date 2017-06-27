/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Setup.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Aug 2016

#ifndef eckit_testing_Setup_h
#define eckit_testing_Setup_h

#include "eckit/runtime/Main.h"

namespace eckit {
namespace testing {

//----------------------------------------------------------------------------------------------------------------------

struct Setup {
    Setup() {
        eckit::Main::initialise(boost::unit_test::framework::master_test_suite().argc,
                                boost::unit_test::framework::master_test_suite().argv);

//        eckit::Main::instance().assertDumps(false);
    }
 };

//----------------------------------------------------------------------------------------------------------------------

struct CommSetup : public Setup {
    CommSetup() : Setup() {
    }
 };

//----------------------------------------------------------------------------------------------------------------------

} // namespace testing
} // namespace eckit

#endif
