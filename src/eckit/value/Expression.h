/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Expression.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef Expression_H
#define Expression_H

#include <functional>
#include <memory>

#include "eckit/memory/NonCopyable.h"
#include "eckit/value/Value.h"

//====================================================================

inline const char* opname(const std::negate<eckit::Value>&) {
    return "-";
}
inline const char* opname(const std::multiplies<eckit::Value>&) {
    return "*";
}
inline const char* opname(const std::divides<eckit::Value>&) {
    return "/";
}
inline const char* opname(const std::modulus<eckit::Value>&) {
    return "%";
}
inline const char* opname(const std::plus<eckit::Value>&) {
    return "+";
}
inline const char* opname(const std::minus<eckit::Value>&) {
    return "-";
}
inline const char* opname(const std::greater<eckit::Value>&) {
    return ">";
}
inline const char* opname(const std::equal_to<eckit::Value>&) {
    return "==";
}
inline const char* opname(const std::less<eckit::Value>&) {
    return "<";
}
inline const char* opname(const std::greater_equal<eckit::Value>&) {
    return ">=";
}
inline const char* opname(const std::less_equal<eckit::Value>&) {
    return "<=";
}
inline const char* opname(const std::not_equal_to<eckit::Value>&) {
    return "!=";
}
inline const char* opname(const std::logical_not<eckit::Value>&) {
    return "!";
}
inline const char* opname(const std::logical_and<eckit::Value>&) {
    return "&&";
}
inline const char* opname(const std::logical_or<eckit::Value>&) {
    return "||";
}

//====================================================================

class EvalError : public eckit::Exception {
public:

    EvalError(const std::string& s) : Exception(std::string("EvalError: ") + s) {}
};

template <class T>
class Expression : private eckit::NonCopyable {
    virtual void print(std::ostream&) const = 0;

public:

    virtual eckit::Value eval(T&) const = 0;
    virtual ~Expression() {}
    friend std::ostream& operator<<(std::ostream& s, const Expression<T>& c) {
        c.print(s);
        return s;
    }
};

template <class T, class U>
class CondUnary : public Expression<U> {

    std::unique_ptr<Expression<U> > cond_;

    void print(std::ostream& s) const override { s << opname(T()) << '(' << *cond_ << ')'; }

public:

    CondUnary(Expression<U>* cond) : cond_(cond) {}
    ~CondUnary() override {}
    virtual eckit::Value eval(U& task) const { return T()(cond_->eval(task)); }
};

template <class T, class U>
class CondBinary : public Expression<U> {

    std::unique_ptr<Expression<U> > left_;
    std::unique_ptr<Expression<U> > right_;

    void print(std::ostream& s) const override { s << '(' << *left_ << ' ' << opname(T()) << ' ' << *right_ << ')'; }

public:

    CondBinary(Expression<U>* left, Expression<U>* right) : left_(left), right_(right) {}

    ~CondBinary() override {}
    eckit::Value eval(U& task) const;
};

template <class T, class U>
inline eckit::Value CondBinary<T, U>::eval(U& task) const {
    return T()(left_->eval(task), right_->eval(task));
}

template <class T>
class StringExpression : public Expression<T> {
    std::string str_;
    void print(std::ostream& s) const override { s << str_; }

public:

    StringExpression(const std::string& s) : str_(s) {}
    ~StringExpression() override {}
    virtual eckit::Value eval(T&) const { return eckit::Value(str_); }
};

template <class T>
class NumberExpression : public Expression<T> {
    long long value_;
    void print(std::ostream& s) const override { s << value_; }

protected:

    long long value() const { return value_; }

public:

    NumberExpression(long long n) : value_(n) {}
    ~NumberExpression() override {}
    virtual eckit::Value eval(T&) const { return eckit::Value(value_); }
};

template <class T>
class ListExpression : public Expression<T> {
    std::vector<Expression<T>*> v_;
    void print(std::ostream& s) const override;

public:

    ListExpression();
    ListExpression(const std::vector<Expression<T>*>& v) : v_(v) {}
    ~ListExpression() override;
    virtual eckit::Value eval(T&) const;
};

template <class T>
ListExpression<T>::~ListExpression() {
    for (size_t i = 0; i < v_.size(); i++) {
        delete v_[i];
    }
}

template <class T>
void ListExpression<T>::print(std::ostream& s) const {
    s << '[';
    for (size_t i = 0; i < v_.size(); i++) {
        if (i) {
            s << ',';
        }
        if (v_[i]) {
            s << *v_[i];
        }
        else {
            s << "(null)";
        }
    }
    s << ']';
}

template <class T>
eckit::Value ListExpression<T>::eval(T& t) const {
    std::vector<eckit::Value> v;
    for (size_t i = 0; i < v_.size(); i++) {
        if (v_[i]) {
            v.push_back(v_[i]->eval(t));
        }
    }

    return v;
}

#endif
