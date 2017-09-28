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
/// @date   May 2017

#ifndef eckit_testing_CommSetup_h
#define eckit_testing_CommSetup_h

#include "eckit/runtime/Main.h"
#include "eckit/mpi/Comm.h"

namespace eckit {
namespace testing {

//----------------------------------------------------------------------------------------------------------------------

/// A special Setup for parallel tests because MPI needs to be explicitly finalised on the MacOSX (see ECKIT-166)
/// If we solve that this class should disappear and use Setup directly

struct CommSetup {
    CommSetup() {
        eckit::Main::initialise(boost::unit_test::framework::master_test_suite().argc,
                                boost::unit_test::framework::master_test_suite().argv);
    }

    ~CommSetup() {
        eckit::mpi::finaliseAllComms();
    }
 };

//----------------------------------------------------------------------------------------------------------------------

} // namespace testing
} // namespace eckit

#endif