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

#include <vector>
#include <sstream>
#include <string>

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit {
namespace testing {

//----------------------------------------------------------------------------------------------------------------------

class TestException : public Exception {
public:
    TestException(const std::string& w, const CodeLocation& l) :
        Exception(w, l) {}
};

enum  TestVerbosity { Silent = 0, Summary = 1, AllFailures = 2};
enum  InitEckitMain { NoInitEckitMain = 0, DoInitEckitMain = 1 };

//----------------------------------------------------------------------------------------------------------------------

/// A test is defined by a description and a function pointer.

class Test {

public:  // methods

    Test(const std::string& description, void (*testFn)(std::string&, int&, int)) :
        description_(description),
        testFn_(testFn) {}

    bool run(TestVerbosity v, std::vector<std::string>& failures) {

        subsection_ = "";

        bool success = true;

        int num_sections = 0;

        // n.b. We always try and loop once too many.
        //  - Allows us to continue after a test fails.
        //  - Allows us to go around the loop once, before the SECTION() macros have incremented the count
        //  - Runs the CASE if it has no subsections
        for (int section = 0; section < (num_sections + 1); section++) {

            // The count is incremented by each subsection in the test, so reset it each loop
            num_sections = 0;

            try {
                testFn_(subsection_, num_sections, section);
            } catch (TestException& e) {
                success = false;
                failures.push_back(description());
                if (v >= Summary)
                    eckit::Log::error()  << "Test \"" << description() << "\" failed: "
                                        << e.what() << " @ " << e.location() << std::endl;
            } catch (eckit::Exception& e) {
                success = false;
                failures.push_back(description());
                if (v >= Summary)
                    eckit::Log::error() << "Test \"" << description() << "\" failed with unhandled eckit::Exception: "
                                       << e.what() << " @ " << e.location() << std::endl;
            } catch (std::exception& e) {
                success = false;
                failures.push_back(description());
                if (v >= Summary)
                    eckit::Log::error() << "Test \"" << description() << "\" failed with unhandled exception: "
                                       << e.what() << " @ " << std::endl;
            } catch (...) {
                success = false;
                failures.push_back(description());
                if (v >= Summary)
                    eckit::Log::error() << "Test \"" << description() << "\" failed with unknown unhandled exception."
                                       << std::endl;
            }
        }

        return success;
    }

    std::string description() const {
        if (subsection_.length() != 0) {
            return description_ + " (section: " + subsection_ + ")";
        } else {
            return description_;
        }
    }

private:  // members

    std::string description_;
    std::string subsection_;

    void (* testFn_)(std::string&, int&, int);
};

std::vector<Test>& specification() {
    static std::vector<Test> tests;
    return tests;
}

//----------------------------------------------------------------------------------------------------------------------

/// Tests are registered using a static object, that is initialised before main()

class TestRegister {
public:
    TestRegister(const std::string& description, void (*testFn)(std::string&, int&, int)) {
        specification().push_back(Test(description, testFn));
    }
};

//----------------------------------------------------------------------------------------------------------------------

/// A helper class which wraps c-style arrays and implements comparison operators to compare with other views
/// and STL vectors. Does not copy or modify the original array. Does not work on non-contiguous data sets.
/// Note that STL containers can (and should) use the built-in comparison operators (==, !=, >=, etc.) instead.
template< typename T >
class ArrayView {
public:

    // -- Constructors
    ArrayView(const T* data, size_t size) : data_(data), size_(size) {}
    ArrayView(const T* begin, const T* end) : data_(begin), size_(end-begin) {}
    explicit ArrayView(const std::vector<T> & vec) : data_(&vec[0]), size_(vec.size()) {}

    // -- Accessors
    const T& operator[](int i) const { return data_[i]; }
    const T& at(int i) const { return data_[i]; }
    size_t size() const { return size_; }
    const T * data() const { return data_; }

    // -- Comparison Operators
    template < typename U >
    bool operator==(const ArrayView<U> & other) const {
        return compareEqual_( other.data(), other.size() );
    }

    template < typename U >
    bool operator==(const std::vector<U> & other) const {
        return compareEqual_( &other[0], other.size() );
    }

    template < typename U >
    bool operator!=(const ArrayView<U> & other) const {
        return !compareEqual_( other.data(), other.size() );
    }

    template < typename U >
    bool operator!=(const std::vector<U> & other) const {
        return !compareEqual_( &other[0], other.size() );
    }

    template < typename U >
    bool isApproximatelyEqual(const ArrayView<U> & other, const U tolerance) const {
        return compareApproxEqual_( other.data(), other.size(), tolerance );
    }
    template < typename U >
    bool isApproximatelyEqual(const std::vector<U> & other, const U tolerance) const {
        return compareApproxEqual_( &other[0], other.size(), tolerance );
    }

private:
    // -- Private Methods
    template < typename U >
    bool compareEqual_(const U * data, const size_t size) const {
        if ( size != this->size() ) return false;
        if ( size == 0 ) return true;
        for ( size_t i = 0; i < this->size(); i++ )
            if (data[i] != this->at(i))
                return false;
        return true;
    }

    template< typename U >
    bool compareApproxEqual_(const U * data, const size_t size, const U epsilon) const {
        if ( size != this->size() ) return false;
        if ( size == 0 ) return true;
        for ( size_t i = 0; i < this->size(); i++ ) {
            if ( !eckit::types::is_approximately_equal(this->at(i), data[i], epsilon))
                return false;
        }
        return true;
    }

    // -- Private Fields
    const T* data_;
    const size_t size_;
};

template <typename T>
static ArrayView<T> make_view(const T * data, size_t size) {
    return ArrayView<T>(data, size);
}
template <typename T>
static ArrayView<T> make_view(const T * begin, const T * end) {
    return ArrayView<T>(begin, end);
}
template <typename T>
static ArrayView<T> make_view(const std::vector<T> & source) {
    return ArrayView<T>( source );
}

template< typename U >
static bool is_approximately_equal(const ArrayView<U> a, const ArrayView<U> b, const U epsilon) {
    return a.isApproximatelyEqual(b, epsilon);
}

template< typename U >
static bool is_approximately_equal(const ArrayView<U> a, const std::vector<U> b, const U epsilon) {
    return a.isApproximatelyEqual(b, epsilon);
}

template< typename U >
static bool is_approximately_equal(const std::vector<U> a, const ArrayView<U> b, const U epsilon) {
    return b.isApproximatelyEqual(a, epsilon);
}

// Note: std has no approx_equal comparison for two vectors
template< typename U >
static bool is_approximately_equal(const std::vector<U> a, const std::vector<U> b, const U epsilon) {
    return ArrayView<U>(a).isApproximatelyEqual(b, epsilon);
}

template < typename U >
bool operator==(const std::vector<U> & lhs, const ArrayView<U> & rhs) {
    return rhs == lhs;
}

template < typename U >
bool operator!=(const std::vector<U> & lhs, const ArrayView<U> & rhs) {
    return rhs != lhs;
}

//----------------------------------------------------------------------------------------------------------------------


inline int run(std::vector<Test>& tests, TestVerbosity v = AllFailures) {

    // Keep track of failures
    std::vector<std::string> failures;
    size_t num_tests = tests.size();

    if (num_tests == 0) {
        if (v >= Summary)
            eckit::Log::info() << "Eckit testing found no test cases to run." << std::endl;
        return -1;
    }

    // Suppress noisy exceptions in eckit (we may throw many, and intentionally!!!)
    ::setenv("ECKIT_EXCEPTION_IS_SILENT", "1", true);
    ::setenv("ECKIT_ASSERT_FAILED_IS_SILENT", "1", true);
    ::setenv("ECKIT_SERIOUS_BUG_IS_SILENT", "1", true);
    for (size_t i = 0; i < num_tests; i++) {

        Test& test(tests[i]);

        eckit::Log::info() << "Running case \"" << test.description() << "\" ..." << std::endl;

        test.run(v, failures);
    }

    if (v >= AllFailures) {
        for (size_t i = 0; i < failures.size(); i++) {
            eckit::Log::info() << "\tFAILED: " << failures[i] << std::endl;
        }
    }

    return failures.size();

}

int run_tests_main(std::vector<Test>& tests, int argc, char * argv[], bool initEckitMain = true) {
    if (initEckitMain)
        eckit::Main::initialise(argc, argv);
    eckit::Log::info() << "Running " << tests.size() << " tests:" << std::endl;
    int failures = run(tests);
    eckit::Log::info() << failures << " tests failed out of " << tests.size() << "." << std::endl;
    return failures;
}

int run_tests(std::vector<Test>& tests, int argc, char* argv[]) {
    return run_tests_main( tests, argc, argv );
}

int run_tests(int argc, char* argv[], bool initEckitMain = true) {
    return run_tests_main( specification(), argc, argv, initEckitMain );
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace testing
}  // namespace eckit

// Helper macros for unique naming

#define UNIQUE_NAME2(name, line) UNIQUE_NAME(name, line)
#define UNIQUE_NAME(name, line) name ## line

#ifndef ECKIT_TESTING_SELF_REGISTER_CASES
#define ECKIT_TESTING_SELF_REGISTER_CASES 1
#endif

#if ECKIT_TESTING_SELF_REGISTER_CASES

#define CASE(description) \
void UNIQUE_NAME2(test_, __LINE__) (std::string&, int&, int); \
static eckit::testing::TestRegister UNIQUE_NAME2(test_registration_, __LINE__)(description, &UNIQUE_NAME2(test_, __LINE__)); \
void UNIQUE_NAME2(test_, __LINE__) (std::string& _test_subsection, int& _num_subsections, int _subsection)

#else  // ECKIT_TESTING_SELF_REGISTER_CASES

#define CASE(description, ...) \
    description, [__VA_ARGS__](std::string& _test_subsection, int& _num_subsections, int _subsection)

#endif  // ECKIT_TESTING_SELF_REGISTER_CASES


#define EXPECT(expr) \
    do { \
        if (!(expr)) { \
            throw eckit::testing::TestException("EXPECT condition failed: " #expr, Here()); \
        } \
    } while (false)

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
    } while (false)


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
    } while (false)

#define EXPECT_THROWS(expr) \
    do { \
        try { \
            expr; \
        } catch (...) { \
            break; \
        } \
        throw eckit::testing::TestException("Exception expected but was not thrown", Here()); \
    } while (false)


// Setup no longer does anything. Can be removed where it has been used
#define SETUP(name)

#define SECTION(name) \
    _num_subsections += 1; \
    _test_subsection = (name); \
    if ((_num_subsections - 1) == _subsection) { \
        eckit::Log::info() << "Running section \"" << _test_subsection << "\" ..." << std::endl; \
    } \
    if ((_num_subsections - 1) == _subsection)


//----------------------------------------------------------------------------------------------------------------------


#endif  // eckit_testing_Test_h
