/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/sql/expression/function/FunctionFactory.h"

#include <float.h>
#include <climits>
#include <cmath>

namespace eckit {
namespace sql {
namespace expression {
namespace function {

//----------------------------------------------------------------------------------------------------------------------


template <typename T, int ARITY>
class ArityFunction : public FunctionExpression {
    std::shared_ptr<SQLExpression> clone() const { return std::make_shared<T>(name_, args_); }

public:
    using FunctionExpression::FunctionExpression;
    static int arity() { return ARITY; }
};


template <double (*FN)(double)>
class UnaryFunction : public ArityFunction<UnaryFunction<FN>, 1> {

    double eval(bool& m) const {

        double a0 = this->args_[0]->eval(m);
        if (m)
            return this->missingValue_;

        return FN(a0);
    }

public:
    using ArityFunction<UnaryFunction<FN>, 1>::ArityFunction;
};

template <double (*FN)(double, double)>
class BinaryFunction : public ArityFunction<BinaryFunction<FN>, 2> {

    double eval(bool& m) const {

        double a0 = this->args_[0]->eval(m);
        if (m)
            return this->missingValue_;
        double a1 = this->args_[1]->eval(m);
        if (m)
            return this->missingValue_;

        return FN(a0, a1);
    }

public:
    using ArityFunction<BinaryFunction<FN>, 2>::ArityFunction;
};

template <double (*FN)(double, double, double)>
class TertiaryFunction : public ArityFunction<TertiaryFunction<FN>, 3> {

    double eval(bool& m) const {

        double a0 = this->args_[0]->eval(m);
        if (m)
            return this->missingValue_;
        double a1 = this->args_[1]->eval(m);
        if (m)
            return this->missingValue_;
        double a2 = this->args_[2]->eval(m);
        if (m)
            return this->missingValue_;

        return FN(a0, a1, a2);
    }

public:
    using ArityFunction<TertiaryFunction<FN>, 3>::ArityFunction;
};

template <double (*FN)(double, double, double, double)>
class QuaternaryFunction : public ArityFunction<QuaternaryFunction<FN>, 4> {

    double eval(bool& m) const {

        double a0 = this->args_[0]->eval(m);
        if (m)
            return this->missingValue_;
        double a1 = this->args_[1]->eval(m);
        if (m)
            return this->missingValue_;
        double a2 = this->args_[2]->eval(m);
        if (m)
            return this->missingValue_;
        double a3 = this->args_[3]->eval(m);
        if (m)
            return this->missingValue_;

        return FN(a0, a1, a2, a3);
    }

public:
    using ArityFunction<QuaternaryFunction<FN>, 4>::ArityFunction;
};

template <double (*FN)(double, double, double, double, double)>
class QuinaryFunction : public ArityFunction<QuinaryFunction<FN>, 5> {

    double eval(bool& m) const {

        double a0 = this->args_[0]->eval(m);
        if (m)
            return this->missingValue_;
        double a1 = this->args_[1]->eval(m);
        if (m)
            return this->missingValue_;
        double a2 = this->args_[2]->eval(m);
        if (m)
            return this->missingValue_;
        double a3 = this->args_[3]->eval(m);
        if (m)
            return this->missingValue_;
        double a4 = this->args_[4]->eval(m);
        if (m)
            return this->missingValue_;

        return FN(a0, a1, a2, a3, a4);
    }

public:
    using ArityFunction<QuinaryFunction<FN>, 5>::ArityFunction;
};


//----------------------------------------------------------------------------------------------------------------------

const double R_Earth_km = 180 * 60 / M_PI * 1.852;
const double R_Earth    = 180 * 60 / M_PI * 1.852 * 1000.0;
const double D2R        = M_PI / 180.0;
const double R2D        = 180.0 / M_PI;

inline double abs(double x) {
    return fabs(x);
}

// Note: ODB's trigonometric funcs require args in degrees
// and return degrees (where applicable)
inline double Func_acos(double x) {
    return (R2D * acos(x));
}
inline double Func_asin(double x) {
    return (R2D * asin(x));
}
inline double Func_atan(double x) {
    return (R2D * atan(x));
}
inline double Func_atan2(double x, double y) {
    return (R2D * atan2(x, y));
}
inline double Func_cos(double x) {
    return (cos(D2R * x));
}
inline double Func_sin(double x) {
    return (sin(D2R * x));
}
inline double Func_tan(double x) {
    return (tan(D2R * x));
}

inline double mod(double x, double y) {
    return fmod(x, y);
}
inline double Func_pow(double x, double y) {
    return ((y) == 2 ? (x) * (x) : pow(x, y));
}
inline double ln(double x) {
    return log(x);
}
inline double lg(double x) {
    return log10(x);
}
inline double twice(double x) {
    return 2 * x;
}

const double ZERO_POINT = ((double)273.15e0);

inline double celsius(double x) {
    return x - ZERO_POINT;
}
inline double k2c(double x) {
    return x - ZERO_POINT;
}
inline double kelvin(double x) {
    return x + ZERO_POINT;
}
inline double c2k(double x) {
    return x + ZERO_POINT;
}
inline double c2f(double x) {
    return ((9 * x) / 5) + 32;
}
inline double f2c(double x) {
    return ((x - 32) * 5) / 9;
}
inline double f2k(double x) {
    return c2k(f2c(x));
}
inline double k2f(double x) {
    return c2f(k2c(x));
}
inline double fahrenheit(double x) {
    return c2f(k2c(x));
}

inline double radians(double x) {
    return x * D2R;
}
inline double deg2rad(double x) {
    return x * D2R;
}
inline double degrees(double x) {
    return x * R2D;
}
inline double rad2deg(double x) {
    return x * R2D;
}

inline double speed(double u, double v) {
    return sqrt(u * u + v * v);
}
inline double ff(double u, double v) {
    return speed(u, v);
}
inline double direction(double u, double v) {
    return fmod(Func_atan2(-u, -v) + 360., 360.);
}

/// Distance in meters.
inline double distance(double lat1, double lon1, double lat2, double lon2) {
    return R_Earth * acos(Func_sin(lat1) * Func_sin(lat2) + Func_cos(lat1) * Func_cos(lat2) * Func_cos(lon1 - lon2));
}

inline double km(double x) {
    return R_Earth_km * x;
}

/// in kilometers
inline double km(double lat1, double lon1, double lat2, double lon2) {
    return R_Earth_km * acos(Func_sin(lat1) * Func_sin(lat2) + Func_cos(lat1) * Func_cos(lat2) * Func_cos(lon1 - lon2));
}

/// in kilometers
inline double dist(double reflat, double reflon, double refdist_km, double obslat, double obslon) {
    return (double)(R_Earth_km * acos(Func_cos(reflat) * Func_cos(obslat) * Func_cos(obslon - reflon) + Func_sin(reflat) * Func_sin(obslat)) <= (refdist_km));
}

inline double circle(double x, double x0, double y, double y0, double r) {
    return (Func_pow(x - x0, 2) + Func_pow(y - y0, 2) <= Func_pow(r, 2));
}


inline double rad(double reflat, double reflon, double refdeg, double obslat, double obslon) {
    //    double v (Func_cos(reflat) * Func_cos(obslat) * Func_cos(obslon-reflon) + Func_sin(reflat) * Func_sin(obslat)
    //    );

    //    int digs ( 3 + DBL_MANT_DIG - DBL_MIN_EXP );

    return (acos(Func_cos(reflat) * Func_cos(obslat) * Func_cos(obslon - reflon) + Func_sin(reflat) * Func_sin(obslat)) <= D2R * refdeg)
               ? 1.0
               : 0.0;
}

inline double between(double x, double a, double b) {
    return x >= a && x <= b;
}
inline double not_between(double x, double a, double b) {
    return x < a || x > b;
}
inline double between_exclude_first(double x, double a, double b) {
    return x > a && x <= b;
}
inline double between_exclude_second(double x, double a, double b) {
    return x >= a && x < b;
}
inline double between_exclude_both(double x, double a, double b) {
    return x > a && x < b;
}

double ibits(double X, double Pos, double Len) {

    constexpr int MAXBITS = 32;

    int rc = 0; /* the default */

    X   = trunc(X);
    Pos = trunc(Pos);
    Len = trunc(Len);

    if (X >= INT_MIN && X <= INT_MAX && Pos >= 0 && Pos < MAXBITS && Len >= 1 && Len <= MAXBITS) {

        rc = (int(X) >> int(Pos)) & ((1 << int(Len)) - 1);
    }

    return (double)rc;
}

double negate_double(double n) {
    return -n;
}
double logical_not_double(double n) {
    return !n;
}
double not_equal_to_double(double l, double r) {
    return l != r;
}
double greater_double(double l, double r) {
    return l > r;
}
double greater_equal_double(double l, double r) {
    return l >= r;
}
double less_double(double l, double r) {
    return l < r;
}
double less_equal_double(double l, double r) {
    return l <= r;
}

double plus_double(double l, double r) {
    return l + r;
}
double minus_double(double l, double r) {
    return l - r;
}
double divides_double(double l, double r) {
    return l / r;
}
double ldexp_double(double l, double r) {
    return ldexp(l, r);
}

/// Multiplication is a special case

class MultiplyFunction : public ArityFunction<MultiplyFunction, 2> {

    double eval(bool& m) const {

        bool m0   = false;
        bool m1   = false;
        double a0 = args_[0]->eval(m0);
        double a1 = args_[1]->eval(m1);

        if ((a0 == 0 || a1 == 0) && !(m0 && m1)) {
            return 0;
        }

        if (m0 || m1) {
            m = true;
            return this->missingValue_;
        }

        return a0 * a1;
    }

public:
    using ArityFunction<MultiplyFunction, 2>::ArityFunction;
};

//----------------------------------------------------------------------------------------------------------------------

// Static factories

static FunctionBuilder<UnaryFunction<abs>> absBuilder("abs", "absolute value");
static FunctionBuilder<UnaryFunction<fabs>> fabsBuilder("fabs", "absolute value");

static FunctionBuilder<UnaryFunction<Func_acos>> acosBuilder("acos", "arc cosine");
static FunctionBuilder<UnaryFunction<Func_asin>> asinBuilder("asin", "arc sine");
static FunctionBuilder<UnaryFunction<Func_atan>> atanBuilder("atan", "arc tangent of one variable");
static FunctionBuilder<BinaryFunction<Func_atan2>> atan2Builder("atan2", "arc tangent of param1/param2 (y/x)");
static FunctionBuilder<UnaryFunction<Func_cos>> cosBuilder("cos", "cosine");
static FunctionBuilder<UnaryFunction<Func_sin>> sinBuilder("sin", "sine");
static FunctionBuilder<UnaryFunction<Func_tan>> tanBuilder("tan", "tangent");

static FunctionBuilder<UnaryFunction<exp>> expBuilder("exp", "base-e exponential function, e raised to x");
static FunctionBuilder<UnaryFunction<cosh>> coshBuilder("cosh", "hyperbolic cosine");
static FunctionBuilder<UnaryFunction<sinh>> sinhBuilder("sinh", "hyperbolic sine");
static FunctionBuilder<UnaryFunction<tanh>> tanhBuilder("tanh", "hyperbolic tangent");
static FunctionBuilder<UnaryFunction<log>> logBuilder("log", "natural logarithm");
static FunctionBuilder<UnaryFunction<log10>> log10Builder("log10", "base-10 logarithm");
static FunctionBuilder<UnaryFunction<sqrt>> sqrtBuilder("sqrt", "square root");

static FunctionBuilder<BinaryFunction<mod>> modBuilder("mod", "");
static FunctionBuilder<BinaryFunction<fmod>> fmodBuilder("fmod", "");
static FunctionBuilder<BinaryFunction<Func_pow>> powBuilder("pow", "x ^ y");
static FunctionBuilder<UnaryFunction<ln>> lnBuilder("ln", "log base e");
static FunctionBuilder<UnaryFunction<lg>> lgBuilder("lg", "log base 10");
static FunctionBuilder<UnaryFunction<twice>> twiceBuilder("twice", "");

static FunctionBuilder<UnaryFunction<celsius>> celsiusBuilder("celsius", "");
static FunctionBuilder<UnaryFunction<k2c>> k2cBuilder("k2c", "");
static FunctionBuilder<UnaryFunction<kelvin>> kelvinBuilder("kelvin", "");
static FunctionBuilder<UnaryFunction<c2k>> c2kBuilder("c2k", "");
static FunctionBuilder<UnaryFunction<c2f>> c2fBuilder("c2f", "");
static FunctionBuilder<UnaryFunction<f2c>> f2cBuilder("f2c", "");
static FunctionBuilder<UnaryFunction<f2k>> f2kBuilder("f2k", "");
static FunctionBuilder<UnaryFunction<k2f>> k2fBuilder("k2f", "");
static FunctionBuilder<UnaryFunction<fahrenheit>> fahrenheitBuilder("fahrenheit", "");

static FunctionBuilder<UnaryFunction<radians>> radiansBuilder("radians", "");
static FunctionBuilder<UnaryFunction<deg2rad>> deg2radBuilder("deg2rad", "");
static FunctionBuilder<UnaryFunction<degrees>> degreesBuilder("degrees", "");
static FunctionBuilder<UnaryFunction<rad2deg>> rad2degBuilder("rad2deg", "");

static FunctionBuilder<BinaryFunction<speed>> speedBuilder("speed", "");
static FunctionBuilder<BinaryFunction<ff>> ffBuilder("ff", "");
static FunctionBuilder<BinaryFunction<direction>> directionBuilder("direction", "");
static FunctionBuilder<BinaryFunction<direction>> ddBuilder("dd", "direction");
static FunctionBuilder<BinaryFunction<direction>> dirBuilder("dir", "direction");

static FunctionBuilder<QuaternaryFunction<distance>> distance4Builder("distance", "");
static FunctionBuilder<UnaryFunction<km>> kmBuilder("km", "");
static FunctionBuilder<QuaternaryFunction<km>> km4Builder("km", "");
static FunctionBuilder<QuinaryFunction<dist>> distBuilder("dist", "");
static FunctionBuilder<QuinaryFunction<circle>> circleBuilder("circle", "");
static FunctionBuilder<QuinaryFunction<rad>> rad5Builder("rad", "");

static FunctionBuilder<TertiaryFunction<between>> betweenBuilder("between", "");
static FunctionBuilder<TertiaryFunction<not_between>> not_betweenBuilder("not_between", "");
static FunctionBuilder<TertiaryFunction<between_exclude_first>> between_exclude_firstBuilder("between_exclude_first",
                                                                                             "");
static FunctionBuilder<TertiaryFunction<between_exclude_second>> between_exclude_secondBuilder("between_exclude_second",
                                                                                               "");
static FunctionBuilder<TertiaryFunction<between_exclude_both>> between_exclude_bothBuilder("between_exclude_both", "");
static FunctionBuilder<TertiaryFunction<ibits>> ibitsBuilder("ibits", "");

static FunctionBuilder<UnaryFunction<negate_double>> negate_doubleBuilder("-", "");
static FunctionBuilder<UnaryFunction<logical_not_double>> logical_not_doubleBuilder("not", "logical not");

static FunctionBuilder<BinaryFunction<not_equal_to_double>> not_equal_to_doubleBuilder("<>", "not equal to");
static FunctionBuilder<BinaryFunction<greater_double>> greater_doubleBuilder(">", "greater than");
static FunctionBuilder<BinaryFunction<greater_equal_double>> greater_equal_doubleBuilder(">=", "greater or equal");
static FunctionBuilder<BinaryFunction<less_double>> less_doubleBuilder("<", "less than");
static FunctionBuilder<BinaryFunction<less_equal_double>> less_equal_doubleBuilder("<=", "less or equal");
static FunctionBuilder<BinaryFunction<plus_double>> plus_doubleBuilder("+", "add");
static FunctionBuilder<BinaryFunction<minus_double>> minus_doubleBuilder("-", "subtract");
static FunctionBuilder<BinaryFunction<divides_double>> divides_doubleBuilder("/", "divide");
static FunctionBuilder<BinaryFunction<ldexp_double>> ldexp_doubleBuilder("ldexp_double", "");

static FunctionBuilder<MultiplyFunction> multiplyBuilder("*", "multiply");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
