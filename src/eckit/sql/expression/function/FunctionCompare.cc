/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/StringTools.h"

#include "eckit/sql/LibEcKitSQL.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/function/FunctionExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

//----------------------------------------------------------------------------------------------------------------------

namespace {

template<typename GenericComparison>
bool string_evaluate(const SQLExpression& l, const SQLExpression& r, bool& missing) {

    std::string v1(l.evalAsString(missing));
    if (missing) return false;
    std::string v2(r.evalAsString(missing));
    if (missing) return false;

    v1 = StringTools::trim(v1, "\t\n\v\f\r ");
    v2 = StringTools::trim(v2, "\t\n\v\f\r ");
    return GenericComparison().compare(v1, v2);
}

template<typename GenericComparison, typename LHS, typename RHS>
bool generic_evaluate(const SQLExpression& lhs, const SQLExpression& rhs, bool& missing) {

    double a0 = lhs.eval(missing);
    if (missing) return false;
    double a1 = rhs.eval(missing);
    if (missing) return false;

    return GenericComparison().template compare<LHS, RHS>(reinterpret_cast<const LHS&>(a0),
                                                          reinterpret_cast<const RHS&>(a1));
}

}

//----------------------------------------------------------------------------------------------------------------------

template <typename GenericComparison>
class FunctionCompare : public FunctionExpression {

    bool (*evaluate_)(const SQLExpression& l, const SQLExpression& r, bool& missing);

public:
    using FunctionExpression::FunctionExpression;

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private: // methods

    double eval(bool& missing) const override;
//    std::shared_ptr<SQLExpression> simplify(bool&) override;

    void prepare(SQLSelect& sql) override;
    void updateType(SQLSelect& sql) override;
    void checkUpdateTypes();
};

//----------------------------------------------------------------------------------------------------------------------

template <typename GenericComparison>
std::shared_ptr<SQLExpression> FunctionCompare<GenericComparison>::clone() const {
    return std::make_shared<FunctionCompare<GenericComparison>>(*this);
}

template <typename GenericComparison>
void FunctionCompare<GenericComparison>::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    checkUpdateTypes();
}

template <typename GenericComparison>
void FunctionCompare<GenericComparison>::updateType(SQLSelect& sql) {
    FunctionExpression::updateType(sql);
    checkUpdateTypes();
}

template<typename GenericComparison>
void FunctionCompare<GenericComparison>::checkUpdateTypes() {

    using namespace eckit::sql::type;
    const auto& ltype(this->args_[0]->type()->getKind());
    const auto& rtype(this->args_[1]->type()->getKind());

    if ((ltype == SQLType::stringType) != (rtype == SQLType::stringType)) {
        std::ostringstream ss;
        ss << "Only one type is string in equality comparison \"" << (*this) << "\"";
        throw BadValue(ss.str(), Here());
    }

    if (ltype == SQLType::stringType) {
        ASSERT(rtype == SQLType::stringType);
        evaluate_ = &string_evaluate<GenericComparison>;
    } else if (LibEcKitSQL::instance().treatIntegersAsDoubles()) {
        evaluate_ = &generic_evaluate<GenericComparison, double, double>;
    } else if (ltype == SQLType::integerType || ltype == SQLType::bitmapType) {
        if (rtype == SQLType::integerType || rtype == SQLType::bitmapType) {
            evaluate_ = &generic_evaluate<GenericComparison, int64_t, int64_t>;
        } else {
            evaluate_ = &generic_evaluate<GenericComparison, int64_t, double>;
        }
    } else if (rtype == SQLType::integerType || rtype == SQLType::bitmapType) {
        evaluate_ = &generic_evaluate<GenericComparison, double, int64_t>;
    } else {
        evaluate_ = &generic_evaluate<GenericComparison, double, double>;
    }
}

template <typename GenericComparison>
double FunctionCompare<GenericComparison>::eval(bool& missing) const {
    return (*evaluate_)(*args_[0], *args_[1], missing);
}

/*
 * This simplification is no longer correct. It only works if data types are exactly 8-bit, so it doesn't work
 * with longer strings. Also, as written, this only works with the EQ version
 */
#if 0
template <typename GenericComparison>
std::shared_ptr<SQLExpression> FunctionCompare<GenericComparison>::simplify(bool& changed) {
    std::shared_ptr<SQLExpression> x = FunctionExpression::simplify(changed);
    if (x)
        return x;

    ColumnExpression* a = dynamic_cast<ColumnExpression*>(args_[0].get());
    ColumnExpression* b = dynamic_cast<ColumnExpression*>(args_[1].get());

    if (a && b) {
        return FunctionFactory::instance().build("join", args_[0], args_[1]);
    }

    //
    if (args_[0]->isConstant() && !args_[1]->isConstant())
        std::swap(args_[0], args_[1]);

    return 0;
}
#endif

//----------------------------------------------------------------------------------------------------------------------

struct GenericEqualTo {
    template <typename LHS, typename RHS>
    bool compare(const LHS& l, const RHS& r) {
        return l == r;
    }
};

struct GenericNotEqualTo {
    template <typename LHS, typename RHS>
    bool compare(const LHS& l, const RHS& r) { return l != r; }
};

static FunctionBuilder<FunctionCompare<GenericEqualTo>> eqFunctionBuilder("=", "equal to");
static FunctionBuilder<FunctionCompare<GenericNotEqualTo>> neqFunctionBuilder("<>", "not equal to");

//----------------------------------------------------------------------------------------------------------------------

class FunctionSetCompare : public FunctionExpression {
public:
    using FunctionExpression::FunctionExpression;
    static int arity() { return -1; }

protected:
    bool (*evaluate_)(const SQLExpression& l, const SQLExpression& r, bool& missing);

    void prepare(SQLSelect& sql) override;
    void updateType(SQLSelect& sql) override;
    void checkUpdateTypes();
};

//----------------------------------------------------------------------------------------------------------------------

void FunctionSetCompare::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    checkUpdateTypes();
}

void FunctionSetCompare::updateType(SQLSelect& sql) {
    FunctionExpression::updateType(sql);
    checkUpdateTypes();
}

void FunctionSetCompare::checkUpdateTypes() {

    /// @note - LHS and RHS the wrong way around, as the LHS operator is sqly.y is appended as the _last_ argument
    ///         to this function.
    using namespace eckit::sql::type;
    const auto& rtype(this->args_[0]->type()->getKind());
    const auto& ltype(this->args_[this->args_.size()-1]->type()->getKind());

    if ((ltype == SQLType::stringType) != (rtype == SQLType::stringType)) {
        std::ostringstream ss;
        ss << "Only one type is string in equality comparison \"" << (*this) << "\"";
        throw BadValue(ss.str(), Here());
    }

    if (ltype == SQLType::stringType) {
        ASSERT(rtype == SQLType::stringType);
        evaluate_ = &string_evaluate<GenericEqualTo>;
    } else if (LibEcKitSQL::instance().treatIntegersAsDoubles()) {
        evaluate_ = &generic_evaluate<GenericEqualTo, double, double>;
    } else if (ltype == SQLType::integerType || ltype == SQLType::bitmapType) {
        if (rtype == SQLType::integerType || rtype == SQLType::bitmapType) {
            evaluate_ = &generic_evaluate<GenericEqualTo, int64_t, int64_t>;
        } else {
            evaluate_ = &generic_evaluate<GenericEqualTo, int64_t, double>;
        }
    } else if (rtype == SQLType::integerType || rtype == SQLType::bitmapType) {
        evaluate_ = &generic_evaluate<GenericEqualTo, double, int64_t>;
    } else {
        evaluate_ = &generic_evaluate<GenericEqualTo, double, double>;
    }
}

//----------------------------------------------------------------------------------------------------------------------

class FunctionIN : public FunctionSetCompare {

    std::shared_ptr<SQLExpression> clone() const override {
        return std::make_shared<FunctionIN>(*this);
    }

    double eval(bool& missing) const override {
        const SQLExpression& val(*args_[args_.size()-1]);
        for (int i = 0; i < args_.size()-1; ++i) {
            if ((*evaluate_)(val, *args_[i], missing)) {
                return true;
            }
        }
        return false;
    }

public:
    using FunctionSetCompare::FunctionSetCompare;
};

static FunctionBuilder<FunctionIN> inFunctionBuilder("in");

//----------------------------------------------------------------------------------------------------------------------

class FunctionNOT_IN : public FunctionSetCompare {

    std::shared_ptr<SQLExpression> clone() const override {
        return std::make_shared<FunctionNOT_IN>(*this);
    }

    double eval(bool& missing) const override {
        const SQLExpression& val(*args_[args_.size()-1]);
        for (int i = 0; i < args_.size()-1; ++i) {
            if ((*evaluate_)(val, *args_[i], missing)) {
                return false;
            }
        }
        return true;
    }

public:
    using FunctionSetCompare::FunctionSetCompare;

};

static FunctionBuilder<FunctionNOT_IN> not_inFunctionBuilder("not_in");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
