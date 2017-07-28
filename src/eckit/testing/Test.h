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
#include <sstream>
#include <string>


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

    Test(const std::string& description, void (*testFn)(std::string&)) :
        description_(description),
        testFn_(testFn) {}

    void run() { testFn_(subsection_); }

    std::string description() const {
        if (subsection_.length() != 0) {
            return description_ + " (section: " + subsection_ + ")";
        } else {
            return description_;
        }
    }

private: // members

    std::string description_;
    std::string subsection_;

    void (* testFn_)(std::string&);
};

std::vector<Test>& specification() {
    static std::vector<Test> tests;
    return tests;
}

//----------------------------------------------------------------------------------------------------------------------

/// Tests are registered using a static object, that is initialised before main()

class TestRegister {
public:
    TestRegister(const std::string& description, void (*testFn)(std::string&)) {
        specification().push_back(Test(description, testFn));
    }
};

//----------------------------------------------------------------------------------------------------------------------

enum  TestVerbosity { Silent = 0, Summary = 1, AllFailures = 2};

inline int run( std::vector<Test>& tests, TestVerbosity v = AllFailures) {

    // Keep track of failures
    std::vector<std::string> failures;
    size_t num_tests = tests.size();
    
    // Suppress noisy exceptions in eckit (we may throw many, and intentionally!!!)
    ::setenv("ECKIT_EXCEPTION_IS_SILENT", "1", true);
    ::setenv("ECKIT_ASSERT_FAILED_IS_SILENT", "1", true);
    ::setenv("ECKIT_SERIOUS_BUG_IS_SILENT", "1", true);
    for (size_t i = 0; i < num_tests; i++) {

        Test& test(tests[i]);

        try {
            test.run();
        } catch (TestException& e) {
            if (v >= Summary)
                eckit::Log::info()  << "Test \"" << test.description() << "\" failed: "
                                    << e.what() << " @ " << e.location() << std::endl;
            failures.push_back(test.description());
        } catch (eckit::Exception& e) {
            if (v >= Summary)
                eckit::Log::info() << "Test \"" << test.description() << "\" failed with unhandled eckit::Exception: "
                                   << e.what() << " @ " << e.location() << std::endl;
            failures.push_back(test.description());
        } catch (std::exception& e) {
            if (v >= Summary)
                eckit::Log::info() << "Test \"" << test.description() << "\" failed with unhandled exception: "
                                   << e.what() << " @ " << std::endl;
            failures.push_back(test.description());
        } catch (...) {
            if (v >= Summary)
                eckit::Log::info() << "Test \"" << test.description() << "\" failed with unknown unhandled exception."
                                   << std::endl;
            failures.push_back(test.description());
        }
    }

    if (v >= AllFailures) {
        for (size_t i = 0; i < failures.size(); i++) {
            eckit::Log::info() << "\tFAILED: " << failures[i] << std::endl;
        }
    }

    return failures.size();

}

template<std::size_t N>
inline int run( Test (&specification)[N], TestVerbosity v = AllFailures) {
    return run( std::vector<Test>(specification, specification+N), v );
}

int run_tests_main( std::vector<Test>& tests, int argc, char * argv[], bool initEckitMain = true ) {
    if (initEckitMain)
        eckit::Main::initialise( argc, argv );
    eckit::Log::info() << "Running " << tests.size() << " tests:" << std::endl;
    int failures = run( tests );
    eckit::Log::info() << failures << " tests failed out of " << tests.size() << "." << std::endl;
    return failures;
}

template<std::size_t N>
int run_tests( Test (&specification)[N], int argc, char* argv[] ) {
    return run_tests_main( std::vector<Test>(specification, specification+N), argc, argv);
}

int run_tests( std::vector<Test>& tests, int argc, char* argv[]) {
    return run_tests_main( tests, argc, argv );
}

int run_tests(int argc, char* argv[], bool initEckitMain = true) {
    return run_tests_main( specification(), argc, argv, initEckitMain );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace testing
} // namespace eckit

// Helper macros for unique naming

#define UNIQUE_NAME2( name, line ) UNIQUE_NAME( name, line )
#define UNIQUE_NAME( name, line ) name ## line

#ifndef ECKIT_TESTING_SELF_REGISTER_CASES
#define ECKIT_TESTING_SELF_REGISTER_CASES 1
#endif

#if ECKIT_TESTING_SELF_REGISTER_CASES

#define CASE(description) \
void UNIQUE_NAME2(test_, __LINE__) (std::string&); \
static TestRegister UNIQUE_NAME2(test_registration_, __LINE__)(description, &UNIQUE_NAME2(test_, __LINE__)); \
void UNIQUE_NAME2(test_, __LINE__) (std::string& _test_subsection)

#else // ECKIT_TESTING_SELF_REGISTER_CASES

#define CASE(description, ... ) \
    description, [__VA_ARGS__](std::string& _test_subsection)

#endif //ECKIT_TESTING_SELF_REGISTER_CASES


#define EXPECT(expr) \
    do { \
        if (!(expr)) { \
            throw eckit::testing::TestException("EXPECT condition failed: " #expr, Here()); \
        } \
    } while(false)

#define EXPECT_NOT(expr) \
    EXPECT(!(expr))

#define EXPECT_THROWS_AS(expr, excpt) \
    do { \
        try { \
            expr; \
        } catch (excpt&) { \
            break; \
        } \
        throw eckit::testing::TestException("Expected exception (" #excpt ")not thrown in: " #expr, Here()); \
    } while(false)


// Convert to TestException
#define EXPECT_NO_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (std::exception& e) { \
            std::stringstream ss; \
            ss << "Unexpected exception caught: " << e.what(); \
            throw eckit::testing::TestException(ss.str(), Here()); \
        } catch (...) { \
            throw eckit::testing::TestException("Unexpected and unknown exception caught", Here()); \
        } \
    } while(false)

#define EXPECT_THROWS(expr) \
    do { \
        try { \
            expr; \
        } catch (...) { \
            break; \
        } \
        throw eckit::testing::TestException("Exception expected but was not thrown", Here()); \
    } while(false)



#define SETUP(name) \
    int _test_num = 0; \
    int _test = 0; \
    int _test_count = 1; \
    _test_subsection = "setup: " name; \
    for ( ; _test_num = 0, _test < _test_count; _test++)

#define SECTION(name) \
    _test_num += 1; \
    _test_count = _test_num; \
    _test_subsection = name; \
    if ((_test_num - 1) == _test)


//----------------------------------------------------------------------------------------------------------------------


#endif // eckit_testing_Test_h
