/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Hash.h"
#include "eckit/value/Value.h"

// Disable warnings for old-style casts in these tests. They are intentional
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

namespace eckit {

// Declarations, otherwise private Content.cc
class BadConversion : public Exception {
public:
    BadConversion(const std::string& w, const CodeLocation& loc);
};

class BadComparison : public Exception {
public:
    BadComparison(const std::string& w, const CodeLocation& loc);
};

class BadOperator : public Exception {
public:
    BadOperator(const std::string& w, const CodeLocation& loc);
};

}  // namespace eckit

namespace eckit {
namespace test {
namespace value_helper {

//----------------------------------------------------------------------------------------------------------------------

// Helper functions, so that we can put operator expressions inside callables that can be passed
// into EXPECT_THROWS_AS

Value ValueAdd(const Value& lhs, const Value& rhs) {
    return lhs + rhs;
}
Value ValueSub(const Value& lhs, const Value& rhs) {
    return lhs - rhs;
}
Value ValueMul(const Value& lhs, const Value& rhs) {
    return lhs * rhs;
}
Value ValueDiv(const Value& lhs, const Value& rhs) {
    return lhs / rhs;
}
Value ValueMod(const Value& lhs, const Value& rhs) {
    return lhs % rhs;
}

Value ValueAddSelf(Value& lhs, const Value& rhs) {
    return lhs += rhs;
}
Value ValueSubSelf(Value& lhs, const Value& rhs) {
    return lhs -= rhs;
}
Value ValueMulSelf(Value& lhs, const Value& rhs) {
    return lhs *= rhs;
}
Value ValueDivSelf(Value& lhs, const Value& rhs) {
    return lhs /= rhs;
}
Value ValueModSelf(Value& lhs, const Value& rhs) {
    return lhs %= rhs;
}

Hash* make_hash() {
    return eckit::HashFactory::instance().build("MD5");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace value_helper
}  // namespace test
}  // namespace eckit
