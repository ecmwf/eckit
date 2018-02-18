/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_Results_h
#define mir_stats_Results_h

#include <iosfwd>
#include <map>
#include <string>
#include <vector>


namespace mir {
namespace stats {


typedef std::map< std::string, double > map_quantity_t;
typedef std::map< std::string, size_t > map_counter_t;
typedef std::map< std::string, int >    map_integer_t;

struct results_entry_t {
    map_quantity_t absoluteQuantities_;
    map_quantity_t absoluteSquaredQuantities_;
    map_quantity_t relativeQuantities_;
    map_quantity_t uncomparableQuantities_;
    map_counter_t  counters_;
    map_integer_t  integers_;
};



class Results : protected std::vector< results_entry_t > {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Results(size_t dimensions=1);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    bool compare(
            const Results& other,
            double absoluteTolerance=0,
            double relativeTolerance=0,
            double counterTolerance=0,
            size_t referenceCounter=1 ) const;

    using std::vector< results_entry_t >::size;

    size_t& counter              (const std::string& name, size_t which=0);
    double& absoluteQuantity     (const std::string& name, size_t which=0);
    double& absoluteQuantity2    (const std::string& name, size_t which=0);
    double& relativeQuantity     (const std::string& name, size_t which=0);
    int&    integerQuantity      (const std::string& name, size_t which=0);
    double& uncomparableQuantity (const std::string& name, size_t which=0);

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

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Results& r) {
        r.print(out);
        return out;
    }

};


}  // namespace stats
}  // namespace mir


#endif

