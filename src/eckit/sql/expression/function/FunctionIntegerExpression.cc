/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionIntegerExpression.h"
#include "eckit/utils/Translator.h"
#include "eckit/sql/LibEcKitSQL.h"

#define ftrunc(x) ((x)-fmod((x), 1))
#define F90nint(x) (((x) > 0) ? (int)((x) + 0.5) : (int)((x)-0.5))

namespace eckit {
namespace sql {
namespace expression {
namespace function {

FunctionIntegerExpression::FunctionIntegerExpression(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args),
    returnAsDouble_(LibEcKitSQL::instance().treatIntegersAsDoubles()) {}

double FunctionIntegerExpression::eval(bool& m) const {
    int64_t v = evalInt(m);
    return returnAsDouble_ ? static_cast<double>(v) : reinterpret_cast<const double&>(v);
}

const eckit::sql::type::SQLType* FunctionIntegerExpression::type() const {
    return &eckit::sql::type::SQLType::lookup("integer");
}

void FunctionIntegerExpression::output(SQLOutput& s) const {
    bool missing = false;
    double v     = eval(missing);
    s.outputInt(v, missing);
    // Log::info() << "FunctionIntegerExpression::output: v=" << v << ", missing=" << missing << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

// TODO: This is REALLY the wrong place for this to be.
constexpr double DEFAULT_MDI = 2147483647;

template <int64_t (*FN)(double)>
class MathFunctionIntegerExpression_1 : public FunctionIntegerExpression {

public:  // methods
    MathFunctionIntegerExpression_1(const std::string& name, const expression::Expressions& args) :
        FunctionIntegerExpression(name, args) {
        this->missingValue_ = DEFAULT_MDI;
    }

private:  // methods
    int64_t evalInt(bool& m) const final {
        bool missing = false;
        double v     = args_[0]->eval(missing);
        if (missing) {
            m = missing;
            return this->missingValue_;
        }
        return FN(v);
    }

    std::shared_ptr<SQLExpression> clone() const {
        return std::make_shared<MathFunctionIntegerExpression_1<FN>>(this->name_, this->args_);
    }
};


//----------------------------------------------------------------------------------------------------------------------

inline int64_t year(double x) {
    return ((int)((x) / 10000));
}
inline int64_t month(double x) {
    return (((int)((x) / 100)) % 100);
}
inline int64_t day(double x) {
    return (((int)(x)) % 100);
}
inline int64_t hour(double x) {
    return ((int)((x) / 10000));
}
inline int64_t minute(double x) {
    return (((int)((x) / 100)) % 100);
}
inline int64_t minutes(double x) {
    return minute(x);
}
inline int64_t second(double x) {
    return (((int)(x)) % 100);
}
inline int64_t seconds(double x) {
    return second(x);
}


inline int64_t Func_ftrunc(double x) {
    return static_cast<int64_t>(ftrunc(x));
}
inline int64_t Func_dnint(double x) {
    return static_cast<int64_t>(F90nint(x));
}
inline int64_t Func_dint(double x) {
    return static_cast<int64_t>(ftrunc(x));
}
inline int64_t Func_ceil(double x) {
    return static_cast<int64_t>(ceil(x));
}
inline int64_t Func_floor(double x) {
    return static_cast<int64_t>(floor(x));
}
inline int64_t Func_atoi(double x) {
    return atoi(Translator<double, std::string>()(x).c_str());
}


//----------------------------------------------------------------------------------------------------------------------

/* Static self-registration */

template <int64_t (*FN)(double)>
using IntegerFunctionBuilder = FunctionBuilder<MathFunctionIntegerExpression_1<FN>>;

static IntegerFunctionBuilder<year> yearFunctionBuilder("year");
static IntegerFunctionBuilder<month> monthFunctionBuilder("month");
static IntegerFunctionBuilder<day> dayFunctionBuilder("day");
static IntegerFunctionBuilder<hour> hourFunctionBuilder("hour");
static IntegerFunctionBuilder<minute> minuteFunctionBuilder("minute");
static IntegerFunctionBuilder<minutes> minutesFunctionBuilder("minutes");
static IntegerFunctionBuilder<second> secondFunctionBuilder("second");
static IntegerFunctionBuilder<seconds> secondsFunctionBuilder("seconds");

static IntegerFunctionBuilder<Func_ftrunc> ftruncFunctionBuilder("trunc");
static IntegerFunctionBuilder<Func_dnint> dnintFunctionBuilder("nint");
static IntegerFunctionBuilder<Func_dint> dintFunctionBuilder("int");
static IntegerFunctionBuilder<Func_ceil> ceilFunctionBuilder("ceil");
static IntegerFunctionBuilder<Func_floor> floorFunctionBuilder("floor");
static IntegerFunctionBuilder<Func_atoi> atoiFunctionBuilder("atoi");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
