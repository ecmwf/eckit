/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_Counter_h
#define mir_stats_detail_Counter_h

#include <cstddef>
#include <iosfwd>

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
namespace detail {


/// Counter accounting for missing values, for a single MIRFIeld
class Counter {
private:

    size_t count_;
    size_t missing_;
    size_t countBelowLowerLimit_;
    size_t countAboveUpperLimit_;
    size_t minIndex_;
    size_t maxIndex_;

    double missingValue_;
    double lowerLimit_;
    double upperLimit_;
    double min_;
    double max_;

    bool hasMissing_;
    bool hasLowerLimit_;
    bool hasUpperLimit_;
    bool first_;

    friend std::ostream& operator<<(std::ostream& out, const Counter& r) {
        r.print(out);
        return out;
    }

public:

    Counter(const param::MIRParametrisation&);

    void reset(const data::MIRField&);
    void print(std::ostream&) const;
    bool count(const double&);

    size_t count() const;
    size_t missing() const;
    size_t countBelowLowerLimit() const;
    size_t countAboveUpperLimit() const;

    double min() const;
    size_t minIndex() const;

    double max() const;
    size_t maxIndex() const;

};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
