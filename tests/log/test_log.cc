/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_debug") {
    Log::debug() << "debug message 1" << std::endl;
}

CASE("test_debug_library") {
    Log::debug<LibEcKit>() << "debug message 2" << std::endl;
}

CASE("test_debug_macro") {
    LOG_DEBUG(true, LibEcKit) << "debug message 3" << std::endl;
    LOG_DEBUG(false, LibEcKit) << "debug message 4" << std::endl;
}

CASE("test_info") {
    Log::info() << "info message 1" << std::endl;
}

CASE("test_warning") {
    Log::warning() << "warning message 1" << std::endl;
}

CASE("test_error") {
    Log::error() << "error message 1" << std::endl;
}

CASE("test_panic") {
    Log::panic() << "panic message 1" << std::endl;
}

CASE("test_strerr") {
    LocalPathName p("/tmp/edfpmjq3480hfnsribnzasdfibv");
    p.unlink();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
