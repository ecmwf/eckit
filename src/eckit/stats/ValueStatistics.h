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


#ifndef mir_stats_ValueStatistics_h
#define mir_stats_ValueStatistics_h

#include <iosfwd>
#include <string>


namespace mir {
namespace stats {


class ValueStatistics {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ValueStatistics()                       = default;
    ValueStatistics(ValueStatistics&&)      = default;
    ValueStatistics(const ValueStatistics&) = delete;

    // -- Destructor

    virtual ~ValueStatistics() = default;

    // -- Convertors
    // None

    // -- Operators

    virtual void operator()(const double&);
    virtual void operator()(const float&);
    void operator=(const ValueStatistics&) = delete;

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

    friend std::ostream& operator<<(std::ostream& out, const ValueStatistics& s) {
        s.print(out);
        return out;
    }
};


}  // namespace stats
}  // namespace mir


#endif
