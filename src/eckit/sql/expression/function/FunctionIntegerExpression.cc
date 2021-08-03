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

#define ftrunc(x) ((x)-fmod((x), 1))
#define F90nint(x) (((x) > 0) ? (int)((x) + 0.5) : (int)((x)-0.5))

namespace eckit {
namespace sql {
namespace expression {
namespace function {

FunctionIntegerExpression::FunctionIntegerExpression(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionIntegerExpression::~FunctionIntegerExpression() {}

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

template <double (*FN)(double)>
class MathFunctionIntegerExpression_1 : public FunctionIntegerExpression {

public:  // methods
    MathFunctionIntegerExpression_1(const std::string& name, const expression::Expressions& args) :
        FunctionIntegerExpression(name, args) {
        this->missingValue_ = DEFAULT_MDI;
    }

private:  // methods
    double eval(bool& m) const {
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

inline double year(double x) {
    return (double)((int)((x) / 10000));
}
inline double month(double x) {
    return (double)(((int)((x) / 100)) % 100);
}
inline double day(double x) {
    return (double)(((int)(x)) % 100);
}
inline double hour(double x) {
    return (double)((int)((x) / 10000));
}
inline double minute(double x) {
    return (double)(((int)((x) / 100)) % 100);
}
inline double minutes(double x) {
    return minute(x);
}
inline double second(double x) {
    return (double)(((int)(x)) % 100);
}
inline double seconds(double x) {
    return second(x);
}


inline double Func_ftrunc(double x) {
    return (double)(ftrunc(x));
}
inline double Func_dnint(double x) {
    return (double)(F90nint(x));
}
inline double Func_dint(double x) {
    return (double)(ftrunc(x));
}
inline double Func_ceil(double x) {
    return (double)(ceil(x));
}
inline double Func_floor(double x) {
    return (double)(floor(x));
}
inline double Func_atoi(double x) {
    return (double)atoi(Translator<double, std::string>()(x).c_str());
}


//----------------------------------------------------------------------------------------------------------------------

/* Static self-registration */

template <double (*FN)(double)>
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
