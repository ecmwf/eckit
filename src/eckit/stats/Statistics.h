/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_Statistics_h
#define mir_stats_Statistics_h

#include <cstddef>
#include <iosfwd>
#include <string>

#include "eckit/memory/NonCopyable.h"


namespace mir {
namespace data {
class MIRField;
}
namespace param {
class MIRParametrisation;
}
}


namespace mir {
namespace stats {


class Statistics : public eckit::NonCopyable {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Statistics(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Statistics();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual void execute(const data::MIRField&) = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    const param::MIRParametrisation& parametrisation_;

    // -- Methods

    /// Output
    virtual void print(std::ostream&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Statistics& r) {
        r.print(out);
        return out;
    }

};


class StatisticsFactory {
private:
    std::string name_;
    virtual Statistics* make(const param::MIRParametrisation&) = 0;
protected:
    StatisticsFactory(const std::string&);
    virtual ~StatisticsFactory();
public:
    static void list(std::ostream&);
    static Statistics* build(const std::string&, const param::MIRParametrisation&);
};


template<class T>
class StatisticsBuilder : public StatisticsFactory {
private:
    Statistics* make(const param::MIRParametrisation& param) {
        return new T(param);
    }
public:
    StatisticsBuilder(const std::string& name) : StatisticsFactory(name) {
    }
};


}  // namespace stats
}  // namespace mir


#endif

