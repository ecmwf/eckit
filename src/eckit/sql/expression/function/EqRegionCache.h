/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file EqRegionCache.h
/// ECMWF July 2010
///
/// A PARTITION OF THE UNIT SPHERE INTO REGIONS OF EQUAL AREA AND SMALL DIAMETER
/// Algorithm by Paul Leopardi, School of Mathematics, University of South Wales
///

#ifndef EqRegionCache_H
#define EqRegionCache_H

#include "eckit/sql/expression/function/RegionCache.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/// @todo Move these functions to eckit::maths or eckit::geometry, or remove them at all
///

class EqRegionCache : public RegionCache {
public:

	EqRegionCache();

	~EqRegionCache();

private:
// No copy allowed
    EqRegionCache(const EqRegionCache&);
    EqRegionCache& operator=(const EqRegionCache&);

    virtual double get_resol(const double & val);
    virtual void create_cache(const double &, const int &);

    int gcd(int, int&);
    void eq_caps(int &, int &, double [], int [], int*);
    void bot_cap_region(int &, double &, double []);
    double circle_offset(int &, int &);
    void cap_colats(int &, int &, int &, double &,const int [], double []);
    void round_to_naturals(int &, int &, const double [], int []);
    double area_of_cap(int &, double &);
    double area_of_collar(int &, double, double);
    void ideal_region_list(int &, int &, double &,int &, double []);
    double area_of_ideal_region(int &, int &);
    int num_collars(int &, double &, double);
    double ideal_collar_angle(int &, int &);
    double sradius_of_cap(int &, double&);
    double my_gamma(double &);
    double area_of_sphere(int &);
    double polar_colat(int &, int &);
    double eq_area(const double &);
    double eq_resol(const double&); 
    double eq_n(const double&); 
    void eq_regions(int, int, double []);
    void sphere_region(int &, double []);
    void top_cap_region(int &, double & ,double []);

    // -- Friends
    //friend std::ostream& operator<<(std::ostream& s,const EqRegionCache& p)
    //	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

#endif
