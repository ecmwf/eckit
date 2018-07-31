/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/sql/expression/function/RegionCache.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

static ThreadSingleton<VectorRegionCache> region_cache_;

double mfmod(double x,double y) { double a = x/y; return (a-int(a))*y; }

//==============================================================================
RegionCache::RegionCache()
//==============================================================================
: nboxes_(0),
  resol_(0),
  nbands_(0),
  latband_(0),
  midlat_(0),
  loncnt_(0),
  sum_loncnt_(0),
  stlon_(0),
  deltalon_(0)
{
}

//==============================================================================
RegionCache::~RegionCache() {}
//==============================================================================

//==============================================================================
VectorRegionCache& RegionCache::instance()
//==============================================================================
{
	return region_cache_.instance();
}

//==============================================================================
double RegionCache::get_resol(const double & nval)
//==============================================================================
{
 return -1;
}
//==============================================================================
int RegionCache::interval_bsearch(const double &key,
                                  const int &n,
                                  const double x[ /* with n+1 elements */ ],
                                  const double *delta, /* if present, only x[0] will be used */
                                  const double &add,
                                  const double &sign /* +1 forward and -1 for reverse search */)
//==============================================================================
{
  const double eps = 1.0e-7;
  double Delta = delta ? *delta : 0;
  bool wrap_around = (delta && add != 0) ? true : false;
  double halfDelta = Delta/2;
  double x0 = x[0] + halfDelta;
  double Key = wrap_around ? sign*mfmod(key + halfDelta + add, add) : sign*(key + halfDelta);
  int lo = 0, hi = n-1;
  while (lo <= hi) {
    int k = (lo+hi)/2;
    double xk = wrap_around ? sign*mfmod(x0 + k*Delta + add, add) : sign*(x[k] + halfDelta);
    if (wrap_around && k > 0 && ABS(xk) < eps) xk = add;
    if (Key < xk) {
      // Search the lower section
      hi = k-1;
    }
    else {
      int kp1 = k+1;
      double xkp1 = wrap_around ? sign*mfmod(x0 + kp1*Delta + add, add) : sign*(x[kp1] + halfDelta);
      if (wrap_around && ABS(xkp1) < eps) xkp1 = add;
      if (Key > xkp1) {
    // Search the upper section
    lo = kp1;
      }
      else {
    // The interval has been found
    return k;
      }
    }
  }
 return -1;
}

//==============================================================================
int RegionCache::find_lonbox(const int & jb, const double & lon,
                               double *midlon, double *leftlon, double *rightlon)
//==============================================================================
{
  const double three_sixty = 360;
  double mid = 0; //odb::MDI::realMDI();
  double left = 0; //odb::MDI::realMDI();
  double right = 0; //odb::MDI::realMDI();
  int boxid = -1;
  if (jb >= 0 && jb < *nbands_) {
    double Lon = mfmod(lon + three_sixty, three_sixty);
    double deltalon = deltalon_[jb];
    if (last_->boxid >= 0) {
      int k = last_->lonbox;
      double startlon = stlon_[jb] + k * deltalon;
      double endlon = stlon_[jb] + (k+1) * deltalon;
      startlon = mfmod(startlon + three_sixty, three_sixty);
      endlon = mfmod(endlon + three_sixty, three_sixty);
      if (endlon >= startlon) {
        if (Lon >= startlon && Lon < endlon) boxid = last_->boxid;
      } else {
        if ((Lon >= 0 && Lon < endlon) || (Lon >= startlon && Lon < three_sixty)) boxid = last_->boxid;
      }
      }

      if (boxid == -1) {
        const double *stlon = &stlon_[jb];
        int loncnt = loncnt_[jb];
        int lonbox = interval_bsearch(lon, loncnt, stlon, &deltalon, three_sixty, +1);
        if (lonbox != -1) {
          int k = lonbox;
          left = (*stlon) + k * deltalon;
          mid = (*stlon) + (k+0.5) * deltalon;
          right = (*stlon) + (k+1) * deltalon;

          last_->left  = left  = mfmod(left + three_sixty, three_sixty);
          last_->mid   = mid   = mfmod(mid + three_sixty, three_sixty);
          last_->right = right = mfmod(right + three_sixty, three_sixty);

          last_->jb = jb;
          last_->lonbox = k;
          last_->boxid = boxid = sum_loncnt_[jb] + k;
        }
      } else {
        left  = last_->left;
        mid   = last_->mid;
        right = last_->right;
      }
    }
   if (midlon) {
      const double one_eighty = 180;
      if (mid > one_eighty) mid -= three_sixty;
      *midlon = mid;
    }
    if (leftlon) *leftlon = left;
    if (rightlon) *rightlon = right;
    return boxid;
}
//==============================================================================
int RegionCache::find_latband(const double & lat) {
//==============================================================================
  int jb;
  int lastjb = last_->jb;
  if (lastjb >= 0 &&
      /* Note: In the following we go from N->S pole, not S->N ==> thus the minus sign!! */
      -lat >= -latband_[lastjb] && -lat < -latband_[lastjb+1]) {
    jb = lastjb;
  } else if (lastjb == *nbands_ - 1 && lat == latband_[lastjb+1]) {
    /* Exactly at the South pole */
    jb = lastjb;
  }  else {
    jb = interval_bsearch(lat, *nbands_,latband_, NULL, 0, -1);
    if (jb != -1) {
      // New latitude band found
      last_->jb = jb;
     // Don't know anything about the final box numbers yet
      last_->lonbox = -1;
      last_->boxid = -1;
    }
  }

  return jb;
}

//==============================================================================
double RegionCache::get_midlat(const double & resol, const double & lat)
//==============================================================================
{
   double res = 0; //odb::MDI::realMDI(); // should be initialised to missing
   get_cache(resol);
   int jb = find_latband(lat);
    if (jb >= 0 && jb < *nbands_) {
      res = midlat_[jb];
    }
    return res;
}

//==============================================================================
double RegionCache::get_midlon(const double & resol, const double & lat, const double & lon)
//==============================================================================
{
   double res = 0; //odb::MDI::realMDI(); // should be initialised to missing
   get_cache(resol);
   int jb = find_latband(lat);
   int boxid = find_lonbox(jb, lon, &res, NULL, NULL);
   boxid = boxid;

   return res;
}

//==============================================================================
void RegionCache::get_cache(const double & nval) 
//==============================================================================
{
   double resol=get_resol(nval);
   bool cache_save=true;
  int n = resol;
// We need to check whether it was already done for this resolution
   VectorRegionCache::const_iterator it_cache = instance().begin();

   while (it_cache != instance().end() && cache_save) {
     RegionCache* cache = const_cast<RegionCache*>(*it_cache);
     if (resol == *(cache->resol_)) {
       cache_save=false; // not need to do it because we have it already!
       kind_  = cache->kind_;
       resol_ = cache->resol_;
       nbands_= cache->nbands_;
       latband_ = cache->latband_;
       midlat_ = cache->midlat_;
       loncnt_ = cache->loncnt_;
       stlon_ = cache->stlon_;
       deltalon_ = cache->deltalon_;
       nboxes_ = cache->nboxes_;
       sum_loncnt_ =  cache->sum_loncnt_;
       last_ = cache->last_;
     } else {
        ++it_cache;
     }
   }
  if (cache_save) 
      create_cache(resol, n);

}
//==============================================================================
void RegionCache::create_cache(const double & resol, const int & n)
//==============================================================================
{
// to be overloaded because it is specific to the grid
}

//==============================================================================
void RegionCache::put_cache(const RegionCacheKind & kind, const double &resol, const int &nb, double latband[], 
                                         double midlat[], double stlon[], 
                                         double deltalon[], int loncnt[])
//==============================================================================
{
VectorRegionCache *p = &(region_cache_.instance());
  int nelm=p->size();
  p->resize(nelm+1);
  p->at(nelm)= new RegionCache;
  p->at(nelm)->kind_ = new RegionCacheKind;
  *(p->at(nelm)->kind_) = kind;
  p->at(nelm)->resol_ = new double;
  *(p->at(nelm)->resol_) = resol;
  p->at(nelm)->nbands_ = new int;
  *(p->at(nelm)->nbands_) = nb;
  resol_ = p->at(nelm)->resol_;
  nbands_=p->at(nelm)->nbands_;
  kind_ = p->at(nelm)->kind_;
  p->at(nelm)->latband_ = latband; 
  p->at(nelm)->midlat_ = midlat;
  p->at(nelm)->loncnt_ = loncnt;
  p->at(nelm)->stlon_ = stlon;
  p->at(nelm)->deltalon_ = deltalon;
  latband_ = latband;
  midlat_ = midlat;
  loncnt_ = loncnt;
  stlon_ = stlon;
  deltalon_ = deltalon;

  {
   int jb, sum = 0;
    p->at(nelm)->sum_loncnt_ = new int[nb];
    for (jb=0; jb<nb; jb++) {
      p->at(nelm)->sum_loncnt_[jb] = sum;
      sum += loncnt[jb];
    }
    p->at(nelm)->nboxes_ = new int;
    *(p->at(nelm)->nboxes_) = sum;
    nboxes_ = p->at(nelm)->nboxes_;
    sum_loncnt_ =  p->at(nelm)->sum_loncnt_;
  }
  p->at(nelm)->last_ = new Last;
  p->at(nelm)->last_->jb = -1;
  p->at(nelm)->last_->lonbox = -1;
  p->at(nelm)->last_->boxid = -1;
  last_ = p->at(nelm)->last_;

}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

