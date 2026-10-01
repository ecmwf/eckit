// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
