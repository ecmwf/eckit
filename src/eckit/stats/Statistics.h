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


class Statistics {
public:
    // -- Constructors

    Statistics(const Parametrisation&);
    Statistics(const Statistics&) = delete;

    // -- Destructor

    virtual ~Statistics();

    // -- Operators

    void operator=(const Statistics&) = delete;

    // -- Methods

    virtual void execute(const Field&) = 0;

protected:
    // -- Members

    const Parametrisation& parametrisation_;

    // -- Methods

    /// Output
    virtual void print(std::ostream&) const = 0;

private:
    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Statistics& r) {
        r.print(out);
        return out;
    }
};


class StatisticsFactory {
private:
    std::string name_;
    virtual Statistics* make(const Parametrisation&) = 0;

    StatisticsFactory(const StatisticsFactory&)            = delete;
    StatisticsFactory& operator=(const StatisticsFactory&) = delete;

protected:
    StatisticsFactory(const std::string&);
    virtual ~StatisticsFactory();

public:
    static void list(std::ostream&);
    static Statistics* build(const std::string&, const Parametrisation&);
};


template <class T>
class StatisticsBuilder : public StatisticsFactory {
private:
    Statistics* make(const Parametrisation& param) override { return new T(param); }

public:
    StatisticsBuilder(const std::string& name) : StatisticsFactory(name) {}
};


}  // namespace eckit::stats
