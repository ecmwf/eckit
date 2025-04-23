/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>


namespace eckit {
class Field;
class Parametrisation;
}  // namespace eckit


namespace eckit::stats {


class Method {
public:
    // -- Constructors

    Method(const Parametrisation&);
    Method(const Method&) = delete;

    // -- Destructor

    virtual ~Method();

    // -- Operators

    void operator=(const Method&) = delete;

    // -- Methods

    virtual void resize(size_t)         = 0;
    virtual void execute(const Field&)  = 0;
    virtual void mean(Field&) const     = 0;
    virtual void variance(Field&) const = 0;
    virtual void stddev(Field&) const   = 0;

protected:
    // -- Members

    const Parametrisation& parametrisation_;

    // -- Methods

    /// Output
    virtual void print(std::ostream&) const = 0;


    friend std::ostream& operator<<(std::ostream& out, const Method& r) {
        r.print(out);
        return out;
    }
};


class MethodFactory {
private:
    std::string name_;
    virtual Method* make(const Parametrisation&) = 0;

    MethodFactory(const MethodFactory&)            = delete;
    MethodFactory& operator=(const MethodFactory&) = delete;

protected:
    MethodFactory(const std::string&);
    virtual ~MethodFactory();

public:
    static void list(std::ostream&);
    static Method* build(const std::string&, const Parametrisation&);
};


template <class T>
class MethodBuilder : public MethodFactory {
private:
    Method* make(const Parametrisation& param) override { return new T(param); }

public:
    MethodBuilder(const std::string& name) : MethodFactory(name) {}
};


}  // namespace eckit::stats
