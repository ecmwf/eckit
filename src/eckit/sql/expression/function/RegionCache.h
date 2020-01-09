/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file RegionCache.h
/// ECMWF July 2010

#ifndef RegionCache_H
#define RegionCache_H

#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/sql/expression/function/piconst.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

static const int dim=2;
static const double min_resol = 0.1e0;
static const double sphere_area = piconst::four_pi; /* Actually: 4 * pi * R^2 */

#define NINT(x) F90nint(x)

/* Fortran90 compatible NINT-function */
#define F90nint(x) ( ((x) > 0) ? (int)((x) + 0.5) : (int)((x) - 0.5) )

#define R2D(x) ( (180/piconst::pi) * ( ((x) >  piconst::pi) ? ((x) - 2*piconst::pi) : (x) ) )

typedef enum { eq_cache_kind = 1, rgg_cache_kind = 2 } RegionCacheKind;

class RegionCache;
typedef std::vector<RegionCache *> VectorRegionCache;

// To store results from the last find_latband, find_lonbox
   class Last {
    public:
      int jb;
      int lonbox;
      int boxid;
      double left, mid, right;
   };

class RegionCache : private eckit::NonCopyable {
public:

	RegionCache();
    virtual ~RegionCache();

 	static VectorRegionCache &  instance();

    double get_midlat(const double &, const double &);
    double get_midlon(const double &, const double &, const double &);

private: // members
    RegionCacheKind *kind_; // type of cache (rgg, eq_boxes)
    int *nboxes_;        // Actual number of boxes
    double *resol_;      // Approximate resolution in degrees at Equator
    int *nbands_;        // number of latitude bands
    double *latband_;   // starting latitudes for each latitude band : size nbands+1 
    double *midlat_;    // mid latitudes for each latitude band : size nbands 
    int *loncnt_;       // # of longitude boxes for each latitude band : size nbands
    int *sum_loncnt_;   // Sum of (longitude) boxes BEFORE this latitude band : size nbands
    double *stlon_;     // starting longitudes for each latitude band : size nb 
    double *deltalon_;  // longitudinal delta for each latitude band : size nb 
    Last *last_;

private: // methods
     virtual double get_resol(const double & val);
     virtual void create_cache(const double &, const int &);
     void get_cache(const double &);
     void put_cache(const RegionCacheKind & kind,const  double &,const  int &,double [],double [],double [],
                    double [],int []);
     int find_latband(const double &);
     int find_lonbox(const int &, const double &, double *, double *, double *);
     int interval_bsearch(const double &, const int &, const double [], const double *, const double &, const double &);

     friend class EqRegionCache;
     friend class RggRegionCache;
	//friend std::ostream& operator<<(std::ostream& s,const RegionCache& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

#endif
