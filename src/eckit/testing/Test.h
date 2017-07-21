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

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/exception/Exceptions.h"

#include <vector>


namespace eckit {
namespace testing {

//----------------------------------------------------------------------------------------------------------------------

class TestException : public Exception {
public:
    TestException(const std::string& w, const CodeLocation& l) :
        Exception(w, l) {}
};

//----------------------------------------------------------------------------------------------------------------------

/// A test is defined by a description and a function pointer.

class Test {

public: // methods

    Test(const std::string& description, void (*testFn)()) :
        description_(description),
        testFn_(testFn) {}

    void run() const { testFn_(); }

    const std::string& description() const { return description_; }

private: // members

    std::string description_;

    void (* testFn_)();
};

std::vector<Test>& specification() {
    static std::vector<Test> tests;
    return tests;
}

//----------------------------------------------------------------------------------------------------------------------

/// Tests are registered using a static object, that is initialised before main()

class TestRegister {
public:
    TestRegister(const std::string& description, void (*testFn)()) {
        specification().push_back(Test(description, testFn));
    }
};

//----------------------------------------------------------------------------------------------------------------------

int run_tests(int argc, char* argv[]) {

    eckit::Main::initialise(argc, argv);

    eckit::Log::info() << "Running " << specification().size() << " tests: " << std::endl;

    // Keep track of failures

    std::vector<std::string> failures;
    size_t num_tests = specification().size();

    // Suppress noisy exceptions in eckit (we may throw many, and intentionally!!!)

    ::setenv("ECKIT_EXCEPTION_IS_SILENT", "1", true);
    ::setenv("ECKIT_ASSERT_FAILED_IS_SILENT", "1", true);

    for (size_t i = 0; i < num_tests; i++) {

        const Test& test(specification()[i]);

//        eckit::Log::info() << "Running test: " << test.description() << std::endl;
//        eckit::Log::info() << "." << std::flush;
        try {
            test.run();
        } catch (const TestException& e) {
            eckit::Log::info() << "Test failed: " << test.description()
                               << ": " << e.what() << std::endl;
            failures.push_back(test.description());
        } catch (...) {
            eckit::Log::info() << "Unhandled exception caught in test: "
                               << test.description() << std::endl;
            failures.push_back(test.description());
        }
    }

    eckit::Log::info() << std::endl << "Ran " << num_tests << " tests" << std::endl;
    eckit::Log::info() << num_tests - failures.size() << " succeeded" << std::endl;
    eckit::Log::info() << failures.size() << " failed ..." << std::endl;
    eckit::Log::info() << std::endl;
    for (size_t i = 0; i < failures.size(); i++) {
        eckit::Log::info() << "    " << failures[i] << std::endl;
    }

    return failures.empty() ? 0 : 1;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace testing
} // namespace eckit

// Helper macros for unique naming

#define UNIQUE_NAME2( name, line ) UNIQUE_NAME( name, line )
#define UNIQUE_NAME( name, line ) name ## line


#define CASE(description) \
void UNIQUE_NAME2(test_, __LINE__) (); \
static TestRegister UNIQUE_NAME2(test_registration_, __LINE__)(description, &UNIQUE_NAME2(test_, __LINE__)); \
void UNIQUE_NAME2(test_, __LINE__) ()


#define EXPECT(expr) \
    do { \
        if (!(expr)) { \
            throw TestException("EXPECT condition failed: " #expr, Here()); \
        } \
    } while(false)


#define EXPECT_THROWS_AS(expr, excpt) \
    do { \
        try { \
            expr; \
        } catch (excpt&) { \
            break; \
        } \
        throw TestException("Expected exception (" #excpt ")not thrown in: " #expr, Here()); \
    } while(false)



//----------------------------------------------------------------------------------------------------------------------


#endif // eckit_testing_Test_h
