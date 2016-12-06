/*
 * (C) Copyright 1996-2016 ECMWF.
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


struct results_entry_t {
    std::map< std::string, size_t > counters_;
    std::map< std::string, double > absoluteQuantities_;
    std::map< std::string, double > absoluteSquaredQuantities_;
    std::map< std::string, double > relativeQuantities_;
    std::map< std::string, int >    integerQuantities_;
    std::map< std::string, double > uncomparableQuantities_;
};



class Results : protected std::vector< results_entry_t > {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Results(size_t dimensions=1) {
        if (dimensions) {
            resize(dimensions);
        }
    }

    // -- Destructor

    virtual ~Results() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    using std::vector< results_entry_t >::size;
    using std::vector< results_entry_t >::resize;

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

    virtual void print(std::ostream&) const;

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

