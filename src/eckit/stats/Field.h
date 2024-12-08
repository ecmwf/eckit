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

#include <iosfwd>
#include <string>


namespace eckit {
class JSON;
}

namespace eckit::param {
class MIRParametrisation;
}  // namespace eckit::param


namespace eckit::stats {


class Field {
public:
    // -- Constructors

    explicit Field(const param::MIRParametrisation&);

    Field(const Field&) = delete;
    Field(Field&&)      = delete;

    // -- Destructor

    virtual ~Field();

    // -- Operators

    void operator=(const Field&) = delete;
    void operator=(Field&&)      = delete;

    // -- Methods

    virtual double value() const                             = 0;
    virtual void count(const double&)                        = 0;
    virtual void reset(double missingValue, bool hasMissing) = 0;

protected:
    // -- Methods

    virtual void json(eckit::JSON&) const   = 0;
    virtual void print(std::ostream&) const = 0;

    static void json_tv(eckit::JSON&, const std::string& type, double value);

private:
    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Field& r) {
        r.print(out);
        return out;
    }

    friend eckit::JSON& operator<<(eckit::JSON& out, const Field& r) {
        r.json(out);
        return out;
    }
};


class FieldFactory {
    std::string name_;
    virtual Field* make(const param::MIRParametrisation&) = 0;

    FieldFactory(const FieldFactory&)            = delete;
    FieldFactory& operator=(const FieldFactory&) = delete;

protected:
    FieldFactory(const std::string&);
    virtual ~FieldFactory();

public:
    static void list(std::ostream&);
    static Field* build(const std::string&, const param::MIRParametrisation&);
};


template <class T>
class FieldBuilder : public FieldFactory {
    Field* make(const param::MIRParametrisation& param) override { return new T(param); }

public:
    FieldBuilder(const std::string& name) : FieldFactory(name) {}
};


}  // namespace eckit::stats
