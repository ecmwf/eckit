/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file RggRegionCache.h
/// ECMWF July 2010

#ifndef RggRegionCache_H
#define RggRegionCache_H

//#include <vector>

#include "eckit/sql/expression/function/RegionCache.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class RggRegionCache : public RegionCache {
public:
	RggRegionCache();
	~RggRegionCache();

private:
// No copy allowed
    RggRegionCache(const RggRegionCache&);
    RggRegionCache& operator=(const RggRegionCache&);

// -- Overridden methods
    virtual double get_resol(const double & val);
    virtual void create_cache(const double &, const int &);

// -- Class methods
    int * read_rtablel_2_file(const int &, int *, int *);
    int gauaw(double [], double [], const int &);
    void bsslzr(double [], const int &); 
};

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

#endif
