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


namespace mir {
namespace stats {


namespace {


template< typename T >
void pretty_print_map(std::ostream& out, const char* sep, T& m) {
    for (typename T::const_iterator j = m.begin(); j != m.end(); ++j) {
        out << sep << j->first << ":\t" << j->second;
    }
}


}  // (anonymous namespace)


void Results::print(std::ostream& out) const {
    out << "Results[\n";
    for (size_t i = 0; i < size(); ++i) {
        const char* sep = ",\n\t\t";
        out << "\t" << (i+1) << "/" << size() << "[";

        pretty_print_map(out, sep, operator[](i).counters_);
        pretty_print_map(out, sep, operator[](i).absoluteQuantities_);
        pretty_print_map(out, sep, operator[](i).absoluteSquaredQuantities_);
        pretty_print_map(out, sep, operator[](i).relativeQuantities_);
        pretty_print_map(out, sep, operator[](i).integerQuantities_);
        pretty_print_map(out, sep, operator[](i).uncomparableQuantities_);

        out << "\n\t]\n";
    }
    out << "]";
}


}  // namespace stats
}  // namespace mir

