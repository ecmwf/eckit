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


namespace mir::param {
class MIRParametrisation;
}  // namespace mir::param


namespace mir::stats {


class Distribution {
public:
    // -- Constructors

    Distribution();
    Distribution(const Distribution&) = delete;

    // -- Destructor

    virtual ~Distribution();

    // -- Convertors
    // None

    // -- Operators
    Distribution& operator=(const Distribution&) = delete;
    virtual double operator()() const            = 0;
    virtual void reset()                         = 0;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // NOne

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Distribution& p) {
        p.print(s);
        return s;
    }
};


class DistributionFactory {
    DistributionFactory(const DistributionFactory&)              = delete;
    void operator=(const DistributionFactory&)                   = delete;
    virtual Distribution* make(const param::MIRParametrisation&) = 0;
    const std::string name_;

protected:
    DistributionFactory(const std::string& name);
    virtual ~DistributionFactory();

public:
    static Distribution* build(const std::string&);
    static void list(std::ostream&);
};


template <class T>
class DistributionBuilder : public DistributionFactory {
    Distribution* make(const param::MIRParametrisation& param) override { return new T(param); }

public:
    DistributionBuilder(const std::string& name) : DistributionFactory(name) {}
};


}  // namespace mir::stats
