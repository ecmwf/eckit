/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Dec 2016


#include "mir/stats/Results.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/JSON.h"
#include "mir/config/LibMir.h"


namespace mir {
namespace stats {


namespace {


template< typename MAP >
bool compare_map_keys(MAP& a, MAP& b) {
    bool cmp = true;

    // check for matching a keys in b, in which case compare the values (exactly)
    for (typename MAP::const_iterator i = a.begin(); i != a.end(); ++i) {
        typename MAP::const_iterator j = b.find(i->first);
        if (j == b.end()) {
            eckit::Log::debug<LibMir>() << "Results::compare: key not found: '" << i->first << "'" << std::endl;
            cmp = false;
        }
    }

    // check for matching b keys in a
    for (typename MAP::const_iterator j = b.begin(); j != b.end(); ++j) {
        typename MAP::const_iterator i = a.find(j->first);
        if (i == a.end()) {
            cmp = false;
        }

    }
    return cmp;
}


bool compare_absolute_quantities(const map_quantity_t& a, const map_quantity_t& b, double tolerance) {
    return true;
}


bool compare_relative_quantities(const map_quantity_t& a, const map_quantity_t& b, double tolerance) {
    return true;
}


bool compare_counters(const map_counter_t& a, const map_counter_t& b, double relativeTolerance, size_t referenceCounter) {
    return true;
}


bool compare_integers(const map_integer_t& a, const map_integer_t& b) {

    // check for matching a keys in b
    bool cmp = compare_map_keys(a, b);

    // compare the maps values (exactly)
    for (map_integer_t::const_iterator i = a.begin(), j; i != a.end(); ++i) {
        if ((j = b.find(i->first)) != b.end()) {
            int a_value = i->second;
            int b_value = j->second;
            cmp = cmp && (a_value == b_value);
        }
    }

    return cmp;
}


}  // (anonymous namespace)


Results::Results(size_t dimensions) : vector(dimensions) {
    ASSERT(!empty());
}


bool Results::compare(
        const Results& other,
        double absoluteTolerance,
        double relativeTolerance,
        double counterTolerance,
        size_t referenceCounter ) const {

    bool ret = (size() == other.size());

    for (size_t i = 0; i < size() && i < other.size(); ++i) {

        bool cmp_abs  = compare_absolute_quantities(operator[](i).absoluteQuantities_,        other[i].absoluteQuantities_,        absoluteTolerance);
        bool cmp_abs2 = compare_absolute_quantities(operator[](i).absoluteSquaredQuantities_, other[i].absoluteSquaredQuantities_, absoluteTolerance*absoluteTolerance);
        bool cmp_rel  = compare_relative_quantities(operator[](i).relativeQuantities_,        other[i].relativeQuantities_,        relativeTolerance);
        bool cmp_cnt  = compare_counters(operator[](i).counters_, other[i].counters_, counterTolerance, referenceCounter);
        bool cmp_int  = compare_integers(operator[](i).integers_, other[i].integers_);

        bool cmp = cmp_abs && cmp_abs2 && cmp_rel && cmp_cnt && cmp_int;
        ret = ret && cmp;

        eckit::Log::debug<LibMir>() << "Results::compare " << (i+1) << "/" << size() << ": "
                                    << cmp << " = ("
                                       "(abs="  << cmp_abs  << ") ∧"
                                       "(abs2=" << cmp_abs2 << ") ∧"
                                       "(rel="  << cmp_rel  << ") ∧"
                                       "(cnt="  << cmp_cnt  << ") ∧"
                                       "(int="  << cmp_int  << ")"
                                       ")" << std::endl;

    }
    return ret;
}


size_t& Results::counter(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).counters_[name];
}


double& Results::absoluteQuantity(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).absoluteQuantities_[name];
}


double& Results::absoluteQuantity2(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).absoluteSquaredQuantities_[name];
}


double& Results::relativeQuantity(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).relativeQuantities_[name];
}


int& Results::integerQuantity(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).integers_[name];
}


double& Results::uncomparableQuantity(const std::string& name, size_t which) {
    ASSERT(which < size());
    return operator[](which).uncomparableQuantities_[name];
}


void Results::print(std::ostream& out) const {
    eckit::JSON j(out);
    j.startList();
    for (const auto& m : *this) {
        if (!m.counters_.empty())                  j << m.counters_;
        if (!m.absoluteQuantities_.empty())        j << m.absoluteQuantities_;
        if (!m.absoluteSquaredQuantities_.empty()) j << m.absoluteSquaredQuantities_;
        if (!m.relativeQuantities_.empty())        j << m.relativeQuantities_;
        if (!m.uncomparableQuantities_.empty())    j << m.uncomparableQuantities_;
    }
    j.endList();
}


}  // namespace stats
}  // namespace mir

