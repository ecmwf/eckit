/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#ifndef mir_stats_Statistics_h
#define mir_stats_Statistics_h

#include <iosfwd>
#include "eckit/exception/Exceptions.h"
#include "mir/stats/Results.h"


namespace mir {
namespace data {
class MIRField;
}
}


namespace mir {
namespace stats {


/**
 * @brief Calculate statistics on a MIRField
 */
class Statistics {
public:

    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Statistics(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Statistics() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    /// Calculate statistics
    virtual Results calculate(const data::MIRField&) const = 0;

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

    virtual void print(std::ostream&) const;

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
    // None

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
