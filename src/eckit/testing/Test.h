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
/// @author Simon Smart
/// @date   July 2017

#ifndef eckit_testing_Test_h
#define eckit_testing_Test_h

#include "eckit/runtime/Main.h"
#include "eckit/testing/lest.h"
#include "eckit/log/Log.h"

namespace eckit {
namespace testing {


//----------------------------------------------------------------------------------------------------------------------

using LestTest = lest::test;


template <std::size_t N>
int runTests(LestTest const (&specification)[N], int argc, char* argv[]) {

    eckit::Main::initialise(argc, argv);

    return lest::run(specification, argc, argv, eckit::Log::info());
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace testing
} // namespace eckit

#endif // eckit_testing_Test_h
