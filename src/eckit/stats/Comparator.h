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
class Parametrisation;
namespace stats {
class Field;
}
}  // namespace eckit


namespace eckit::stats {


class Comparator {
public:
    // -- Constructors

    Comparator(const Parametrisation&, const Parametrisation&);
    Comparator(const Comparator&) = delete;

    // -- Destructor

    virtual ~Comparator();

    // -- Operators

    void operator=(const Comparator&) = delete;

    // -- Methods

    virtual std::string execute(const Field&, const Field&) = 0;

protected:
    // -- Members

    const Parametrisation& parametrisation1_;
    const Parametrisation& parametrisation2_;

    // -- Methods

    virtual void print(std::ostream&) const = 0;

private:
    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Comparator& r) {
        r.print(out);
        return out;
    }
};


class ComparatorFactory {
    std::string name_;
    virtual Comparator* make(const Parametrisation&, const Parametrisation&) = 0;

    ComparatorFactory(const ComparatorFactory&)            = delete;
    ComparatorFactory& operator=(const ComparatorFactory&) = delete;

protected:
    ComparatorFactory(const std::string&);
    virtual ~ComparatorFactory();

public:
    static void list(std::ostream&);
    static Comparator* build(const std::string&, const Parametrisation&, const Parametrisation&);
};


template <class T>
class ComparatorBuilder : public ComparatorFactory {
    Comparator* make(const Parametrisation& param1, const Parametrisation& param2) override {
        return new T(param1, param2);
    }

public:
    ComparatorBuilder(const std::string& name) : ComparatorFactory(name) {}
};


}  // namespace eckit::stats
