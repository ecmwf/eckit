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
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/sql/expression/function/EqRegionCache.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

#define regions(i,j,k)   regs  [ ((k)-1)*dim*2     + ((j)-1)*dim     + (i) - 1 ]
#define regions_1(i,j,k) regs_1[ ((k)-1)*(dim-1)*2 + ((j)-1)*(dim-1) + (i) - 1 ]
#define region(i,j) reg[((j)-1)*dim + (i) - 1]

#define swap(T, vi, vj) { register T temp = vi; vi = vj; vj = temp; }

extern double mfmod(double x,double y);

//==============================================================================
EqRegionCache::EqRegionCache()
//==============================================================================
: RegionCache()
{
}

//==============================================================================
EqRegionCache::~EqRegionCache() {}
//==============================================================================

//==============================================================================
int EqRegionCache::gcd(int a, int &b)
//==============================================================================
{
  int m = ABS(a);
  int n = ABS(b);
  if ( m > n ) swap(int, m, n);
  return (m == 0) ? n : gcd(n%m, m);
}


//==============================================================================
void EqRegionCache::bot_cap_region(int &dim, double &a_cap, double reg[])
//==============================================================================
{
    //
    // An array of two points representing the bottom cap of radius a_cap as a region.
    //
  if ( dim == 1 ) {
    region(1,1)=piconst::two_pi-a_cap;
    region(1,2)=piconst::two_pi;
  }
  else if ( dim == 2 ) {
      // sphere_region_1 = sphere_region(dim-1);
      // region = [[sphere_region_1(:,1); pi-a_cap],[sphere_region_1(:,2); pi]];
    region(1,1)=0;
    region(1,2)=piconst::two_pi;
    region(2,1)=piconst::pi-a_cap;
    region(2,2)=piconst::pi;
  }
  else {
    Log::info() << "bot_cap_region: dim > 2 not supported";
  }
}

//==============================================================================
double EqRegionCache::circle_offset(int &n_top, int &n_bot)
//==============================================================================
{
    //
    //CIRCLE_OFFSET Try to maximize minimum distance of center points for S^2 collars
    //
    // Given n_top and n_bot, calculate an offset.
    //
    // The values n_top and n_bot represent the numbers of
    // equally spaced points on two overlapping circles.
    // The offset is given in multiples of whole rotations, and
    // consists of three parts;
    // 1) Half the difference between a twist of one sector on each of bottom and top.
    // This brings the centre points into alignment.
    // 2) A rotation which will maximize the minimum angle between
    // points on the two circles.
    //
  double co =
    (double)(1/(double)(n_bot) - 1/(double)(n_top))/2 +
    (double)(gcd(n_top,n_bot))/(2*(double)(n_top)*(double)(n_bot));
  return co;
}

//==============================================================================
void EqRegionCache::cap_colats(int &dim, int &n, int &n_collars, double &c_polar,
       const int n_regions[/* n_collars+2 */],
       double c_caps[/* n_collars+2 */])
//==============================================================================
{
    //
    //CAP_COLATS Colatitudes of spherical caps enclosing cumulative sum of regions
    //
    // Given dim, N, c_polar and n_regions, determine c_caps,
    // an increasing list of colatitudes of spherical caps which enclose the same area
    // as that given by the cumulative sum of regions.
    // The number of elements is n_collars+2.
    // c_caps[1] is c_polar.
    // c_caps[n_collars+1] is Pi-c_polar.
    // c_caps[n_collars+2] is Pi.
    //

  int subtotal_n_regions, collar_n;
  double ideal_region_area = area_of_ideal_region(dim,n);
  c_caps[0] = c_polar;
  subtotal_n_regions = 1;
  double area;
  for (collar_n = 1; collar_n <= n_collars; collar_n++) {
    subtotal_n_regions += n_regions[collar_n];
    area=subtotal_n_regions*ideal_region_area;
    c_caps[collar_n] = sradius_of_cap(dim,area);
  }
  c_caps[n_collars+1] = piconst::pi;
}

//==============================================================================
void EqRegionCache::round_to_naturals(int &n, int &n_collars,
          const double r_regions[/* n_collars+2 */],
          int n_regions[/* n_collars+2 */])
//==============================================================================
{
    //
    //ROUND_TO_NATURALS Round off a given list of numbers of regions
    //
    // Given N and r_regions, determine n_regions,
    // a list of the natural number of regions in each collar and the polar caps.
    // This list is as close as possible to r_regions, using rounding.
    // The number of elements is n_collars+2.
    // n_regions[1] is 1.
    // n_regions[n_collars+2] is 1.
    // The sum of n_regions is N.
    //
  int j;
  double discrepancy = 0;
  for (j=0; j<n_collars+2; j++) {
    n_regions[j] = NINT(r_regions[j]+discrepancy);
    discrepancy += r_regions[j]-n_regions[j];
  }
}

double EqRegionCache::area_of_cap(int &dim, double &s_cap)
{
    //AREA_OF_CAP Area of spherical cap
    //
    //Syntax
    // area = area_of_cap(dim, s_cap);
    //
    //Description
    // AREA = AREA_OF_CAP(dim, S_CAP) sets AREA to be the area of an S^dim spherical
    // cap of spherical radius S_CAP.
    //
    // The argument dim must be a positive integer.
    // The argument S_CAP must be a real number or an array of real numbers.
    // The result AREA will be an array of the same size as S_CAP.

    ASSERT(dim > 0);
    double area = 0;
    if ( dim == 1 ) {
        area = 2 * s_cap;
    }
    else if ( dim == 2 ) {
        area = piconst::four_pi * pow(sin(s_cap/2),2);
    }
    else {
        Log::info() << "area_of_cap: dim > 2 not supported";
    }
    return area;
}

//==============================================================================
double EqRegionCache::area_of_collar(int &dim, double a_top, double a_bot)
//==============================================================================
{
     //
     //AREA_OF_COLLAR Area of spherical collar
     //
     //Syntax
     // area = area_of_collar(dim, a_top, a_bot);
     //
     //Description
     // AREA = AREA_OF_COLLAR(dim, A_TOP, A_BOT) sets AREA to be the area of
     // an S^dim spherical collar specified by A_TOP, A_BOT, where
     // A_TOP is top (smaller) spherical radius,
     // A_BOT is bottom (larger) spherical radius.
     //
     // The argument dim must be a positive integer.
     // The arguments A_TOP and A_BOT must be real numbers or arrays of real numbers,
     // with the same array size.
     // The result AREA will be an array of the same size as A_TOP.
     //
  return area_of_cap(dim, a_bot) - area_of_cap(dim, a_top);
}

//==============================================================================
void EqRegionCache::ideal_region_list(int &dim, int &n, double &c_polar,
          int &n_collars, double r_regions[/* n_collars+2 */])
//==============================================================================
{
    //
    //IDEAL_REGION_LIST The ideal real number of regions in each zone
    //
    // List the ideal real number of regions in each collar, plus the polar caps.
    //
    // Given dim, N, c_polar and n_collars, determine r_regions,
    // a list of the ideal real number of regions in each collar,
    // plus the polar caps.
    // The number of elements is n_collars+2.
    // r_regions[1] is 1.
    // r_regions[n_collars+2] is 1.
    // The sum of r_regions is N.
    //
  r_regions[0] = 1;
  if ( n_collars > 0 ) {
      //
      // Based on n_collars and c_polar, determine a_fitting,
      // the collar angle such that n_collars collars fit between the polar caps.
      //
    int collar_n;
    double a_fitting = (piconst::pi-2*c_polar)/n_collars;
    double ideal_region_area = area_of_ideal_region(dim,n);
    for (collar_n=1; collar_n<=n_collars; collar_n++) {
      double ideal_collar_area = area_of_collar(dim, c_polar+(collar_n-1)*a_fitting,
                        c_polar+collar_n*a_fitting);
      r_regions[collar_n] = ideal_collar_area / ideal_region_area;
    }
  } /* if ( n_collars > 0 ) */
  r_regions[n_collars+1] = 1;
}

//==============================================================================
double EqRegionCache::area_of_ideal_region(int &dim, int &n)
//==============================================================================
{
    //
    // AREA = AREA_OF_IDEAL_REGION(dim,N) sets AREA to be the area of one of N equal
    // area regions on S^dim, that is 1/N times AREA_OF_SPHERE(dim).
    //
    // The argument dim must be a positive integer.
    // The argument N must be a positive integer or an array of positive integers.
    // The result AREA will be an array of the same size as N.
    //
  double area = area_of_sphere(dim)/n;
  return area;
}


//==============================================================================
int EqRegionCache::num_collars(int &n, double &c_polar, double a_ideal)
//==============================================================================
{
    //
    //NUM_COLLARS The number of collars between the polar caps
    //
    // Given N, an ideal angle, and c_polar,
    // determine n_collars, the number of collars between the polar caps.
    //
  int num_c;
    // n_collars = zeros(size(N));
    // enough = (N > 2) & (a_ideal > 0);
    // n_collars(enough) = max(1,round((pi-2*c_polar(enough))./a_ideal(enough)));
  bool enough = ((n > 2) && (a_ideal > 0)) ? true : false;
  if ( enough ) {
    num_c = NINT((piconst::pi-2*c_polar)/a_ideal);
    if (num_c < 1) num_c = 1;
  }
  else {
    num_c = 0;
  }
  return num_c;
}

//==============================================================================
double EqRegionCache::ideal_collar_angle(int &dim, int &n)
//==============================================================================
{
    //
    //IDEAL_COLLAR_ANGLE The ideal angle for spherical collars of an EQ partition
    //
    //Syntax
    // angle = ideal_collar_angle(dim,N);
    //
    //Description
    // ANGLE = IDEAL_COLLAR_ANGLE(dim,N) sets ANGLE to the ideal angle for the
    // spherical collars of an EQ partition of the unit sphere S^dim into N regions.
    //
    // The argument dim must be a positive integer.
    // The argument N must be a positive integer or an array of positive integers.
    // The result ANGLE will be an array of the same size as N.
    //
  return pow(area_of_ideal_region(dim,n),(1/(double)(dim)));
}

//==============================================================================
double EqRegionCache::my_gamma(double &x)
//==============================================================================
{
  const double p0  =  0.999999999999999990e0;
  const double p1  = -0.422784335098466784e0;
  const double p2  = -0.233093736421782878e0;
  const double p3  =  0.191091101387638410e0;
  const double p4  = -0.024552490005641278e0;
  const double p5  = -0.017645244547851414e0;
  const double p6  =  0.008023273027855346e0;
  const double p7  = -0.000804329819255744e0;
  const double p8  = -0.000360837876648255e0;
  const double p9  =  0.000145596568617526e0;
  const double p10 = -0.000017545539395205e0;
  const double p11 = -0.000002591225267689e0;
  const double p12 =  0.000001337767384067e0;
  const double p13 = -0.000000199542863674e0;
  int n = NINT(x - 2);
  double w = x - (n + 2);
  double y = ((((((((((((p13 * w + p12) * w + p11) * w + p10) *
              w + p9) * w + p8) * w + p7) * w + p6) * w + p5) *
         w + p4) * w + p3) * w + p2) * w + p1) * w + p0;
  int k;
  if (n > 0) {
    w = x - 1;
    for (k = 2; k <= n; k++) w *= (x - k);
  }
  else {
    int nn = -n - 1;
    w = 1;
    for (k=0; k <= nn; k++) y *= (x + k);
  }
  return w / y;
}


//==============================================================================
double EqRegionCache::area_of_sphere(int &dim)
//==============================================================================
{
    //
    // AREA = AREA_OF_SPHERE(dim) sets AREA to be the area of the sphere S^dim
    //
  double power = (double)(dim+1)/(double)2;
  return 2*pow(piconst::pi,power)/my_gamma(power);
}



//==============================================================================
double EqRegionCache::sradius_of_cap(int &dim, double & area)
//==============================================================================
{
    //
    // S_CAP = SRADIUS_OF_CAP(dim, AREA) returns the spherical radius of
    // an S^dim spherical cap of area AREA.
    //
  double radius = 0;
  if ( dim == 1 ) {
    radius = area/2;
  }
  else if ( dim == 2 ) {
    radius = 2*asin(sqrt(area/piconst::pi)/2);
  }
  else {
    Log::info() << "sradius_of_cap: dim > 2 not supported";
  }
  return radius;
}

//==============================================================================
double EqRegionCache::polar_colat(int & dim, int & n)
//==============================================================================
{
   //
   // Given dim and N, determine the colatitude of the North polar spherical cap.
   //
   ASSERT(n > 0);
   double colat = 0;
   if ( n == 1 ) colat=piconst::pi;
   else if ( n == 2 ) colat=piconst::half_pi;
   else if ( n > 2 ) {
       double area = area_of_ideal_region(dim,n);
       colat=sradius_of_cap(dim,area);
   }
   return colat;
}

//==============================================================================
void EqRegionCache::top_cap_region(int &dim, double & a_cap,double reg[])
//==============================================================================
{
    //
    // An array of two points representing the top cap of radius a_cap as a region.
    //
  if ( dim == 1 ) {
    region(1,1)=0;
    region(1,2)=a_cap;
  }
  else if ( dim == 2 ) {
      // sphere_region_1 = sphere_region(dim-1);
      // region = [[sphere_region_1(:,1); 0], [sphere_region_1(:,2); a_cap]];
    region(1,1)=0;
    region(1,2)=piconst::two_pi;
    region(2,1)=0;
    region(2,2)=a_cap;
  }
  else {
    Log::info() << "top_cap_region: dim > 2 not supported";
  }

}

//==============================================================================
void EqRegionCache::sphere_region(int &dim, double reg[])
//==============================================================================
{
  //    
  //   An array of two points representing S^dim as a region.
  //    
  if ( dim == 1 ) {
    region(1,1)=0;
    region(1,2)=piconst::two_pi;
  }
  else if ( dim == 2 ) {
  // 
  //  sphere_region_1 = sphere_region(dim-1);
  //  region = [[sphere_region_1(:,1); 0],[sphere_region_1(:,2); pi]] ;
  //
    region(1,1)=0;
    region(1,2)=piconst::two_pi;
    region(2,1)=0;
    region(2,2)=piconst::pi;
  }
  else {
    Log::info() << "sphere_region: dim > 2 not supported";
  }
}

//==============================================================================
void EqRegionCache::eq_caps(int &dim, int &n, 
	double s_cap[/* n */],
	int n_regions[/* n */],
	int *N_collars)
//==============================================================================
{
  int j;
  double c_polar;
  int n_collars = *N_collars;

  if ( n == 1 ) {
      //
      // We have only one region, which must be the whole sphere.
      //
    s_cap[0]=piconst::pi;
    n_regions[0]=1;
    *N_collars=0;
    return;
  }

  if ( dim == 1 ) {
      //
      // We have a circle. Return the angles of N equal sectors.
      //
      // sector = 1:N;
      //
      // Make dim==1 consistent with dim>1 by
      // returning the longitude of a sector enclosing the
      // cumulative sum of arc lengths given by summing n_regions.
      //
      // s_cap = sector*two_pi/N;
      // n_regions = ones(size(sector));
    for (j=0; j<n; j++) {
      s_cap[j]=(j+1)*piconst::two_pi/(double)n;
      n_regions[j]=1;
    }
    *N_collars=0;
    return;
  }

  if ( dim == 2 ) {
      //
      // Given dim and N, determine c_polar
      // the colatitude of the North polar spherical cap.
      //

    c_polar = polar_colat(dim,n);

      //
      // Given dim and N, determine the ideal angle for spherical collars.
      // Based on N, this ideal angle, and c_polar,
      // determine n_collars, the number of collars between the polar caps.
      //

    n_collars = *N_collars = num_collars(n,c_polar,ideal_collar_angle(dim,n));

      //
      // Given dim, N, c_polar and n_collars, determine r_regions,
      // a list of the ideal real number of regions in each collar,
      // plus the polar caps.
      // The number of elements is n_collars+2.
      // r_regions[1] is 1.
      // r_regions[n_collars+2] is 1.
      // The sum of r_regions is N.
      //
      // r_regions = ideal_region_list(dim,N,c_polar,n_collars)

    {
      double *r_regions = NULL;
      r_regions = new double [n_collars+2];
      ideal_region_list(dim,n,c_polar,n_collars,r_regions);
      
	//
	// Given N and r_regions, determine n_regions,
	// a list of the natural number of regions in each collar and
	// the polar caps.
	// This list is as close as possible to r_regions.
	// The number of elements is n_collars+2.
	// n_regions[1] is 1.
	// n_regions[n_collars+2] is 1.
	// The sum of n_regions is N.
	//
	// n_regions = round_to_naturals(N,r_regions)
      
      round_to_naturals(n,n_collars,r_regions,n_regions);
      delete [] r_regions;
    }
    
      //
      // Given dim, N, c_polar and n_regions, determine s_cap,
      // an increasing list of colatitudes of spherical caps which enclose the same area
      // as that given by the cumulative sum of regions.
      // The number of elements is n_collars+2.
      // s_cap[1] is c_polar.
      // s_cap[n_collars+1] is Pi-c_polar.
      // s_cap[n_collars+2] is Pi
      // 
      // s_cap = cap_colats(dim,N,c_polar,n_regions)

    cap_colats(dim,n,n_collars,c_polar,n_regions,s_cap);
  }
}


//==============================================================================
void EqRegionCache::eq_regions(int dim, int n, double regs[])
//==============================================================================
{
  //
  // 
  //    REGIONS = EQ_REGIONS(dim,N) uses the recursive zonal equal area sphere
  //    partitioning algorithm to partition S^dim (the unit sphere in dim+1
  //    dimensional space) into N regions of equal area and small diameter.
  //   
  //    The arguments dim and N must be positive integers.
  //   
  //    The result REGIONS is a (dim by 2 by N) array, representing the regions
  //    of S^dim. Each element represents a pair of vertex points in spherical polar
  //    coordinates.
  //   
  //    Each region is defined as a product of intervals in spherical polar
  //    coordinates. The pair of vertex points regions(:,1,n) and regions(:,2,n) give
  //    the lower and upper limits of each interval.
     
  double *regs_1 = NULL;
  double *s_cap = NULL, r_top[2], r_bot[2], c_top, c_bot, offset;
  int *n_regions = NULL, collar_n, n_collars, n_in_collar, region_1_n, region_n;
  int i, j, k;

  s_cap = new double[n+2];
  n_regions = new int[n+2];

  if (n == 1) {
    //
    // 
    //  We have only one region, which must be the whole sphere.
    // 
    //  regions(:,:,1)=sphere_region(dim)
    //
    sphere_region(dim,&regions(1,1,1));
  } else {
    //
    //  
    //  Start the partition of the sphere into N regions by partitioning
    //  to caps defined in the current dimension.
    //  
    //  [s_cap, n_regions] = eq_caps(dim,N)
    //


    eq_caps(dim,n,s_cap,n_regions,&n_collars);

    //
    //   
    //  s_cap is an increasing list of colatitudes of the caps.
    // 
    //
    if ( dim == 1 ) {
      //
      //   
      //    We have a circle and s_cap is an increasing list of angles of sectors.
      //   
      //   
      //    Return a list of pairs of sector angles.
      //   
      //
      for (k=1; k<=n; k++)
	for (j=1; j<=2; j++)
	  for (i=1; i<=dim; i++)
	    regions(i,j,k) = 0;

      for (k=2; k<=n; k++)
	regions(1,1,k) = s_cap[k-2];

      for (k=1; k<=n; k++) 
	regions(1,2,k) = s_cap[k-1];
      
    } else {

      //   
      //    We have a number of zones: two polar caps and a number of collars.
      //    n_regions is the list of the number of regions in each zone.
      //   
      //    n_collars = size(n_regions,2)-2
      //   
      //    Start with the top cap
      //   
      //    regions(:,:,1) = top_cap_region(dim,s_cap(1))
      //

      top_cap_region(dim,s_cap[0],&regions(1,1,1));
      region_n = 1;
      
      //
      //    
      //    Determine the dim-regions for each collar
      //   
      //
      if ( dim == 2 ) offset=0;
      for (collar_n=1; collar_n<=n_collars; collar_n++) {
	int size_regions_1_3;
	//
	// c_top is the colatitude of the top of the current collar.
	// 
        c_top = s_cap[collar_n-1];
	
	//
	// c_bot is the colatitude of the bottom of the current collar.
	// 
        c_bot = s_cap[collar_n];
	
	//
	// n_in_collar is the number of regions in the current collar.
	//
	n_in_collar = n_regions[collar_n];
	
	// 
	// The top and bottom of the collar are small (dim-1)-spheres,
	// which must be partitioned into n_in_collar regions.
	// Use eq_regions recursively to partition the unit (dim-1)-sphere.
	// regions_1 is the resulting list of (dim-1)-region pairs.
	//
	// regions_1 = eq_regions(dim-1,n_in_collar)
	// 
        size_regions_1_3 = n_in_collar;
	
	regs_1 = new double[(dim-1)*2*(n-1)];
	eq_regions(dim-1, n_in_collar, regs_1);
	//
	//  Given regions_1, determine the dim-regions for the collar.
	//  Each element of regions_1 is a (dim-1)-region pair for the (dim-1)-sphere.
	//
	if ( dim == 2 ) {
	  //
	  //  The (dim-1)-sphere is a circle
	  //  Offset each sector angle by an amount which accumulates over
	  //  each collar.
	  //
	  for (region_1_n=1; region_1_n<=n_in_collar; region_1_n++) {
	    //
	    //    Top of 2-region
	    //    The first angle is the longitude of the top of
	    //    the current sector of regions_1, and
	    //    the second angle is the top colatitude of the collar
	    //   
	    //    r_top = [mod(regions_1(1,1,region_1_n)+two_pi*offset,two_pi); c_top]
	    //
	    
	    r_top[0]=mfmod(regions_1(1,1,region_1_n)+piconst::two_pi*offset,piconst::two_pi);
	    r_top[1]=c_top;
	    //
	    //   Bottom of 2-region
	    //   The first angle is the longitude of the bottom of
	    //   the current sector of regions_1, and
	    //   the second angle is the bottom colatitude of the collar
	    //  
	    //   r_bot = [mod(regions_1(1,2,region_1_n)+two_pi*offset,two_pi); c_bot]
	    //
	    
	    r_bot[0]=mfmod(regions_1(1,2,region_1_n)+piconst::two_pi*offset,piconst::two_pi);
	    r_bot[1]=c_bot;
	    if ( r_bot[0] <= r_top[0] )
	      r_bot[0] += piconst::two_pi;
	    
	    region_n++;
	    /* regions(:,:,region_n) = [r_top,r_bot] */
	    regions(1,1,region_n) = r_top[0];
	    regions(1,2,region_n) = r_bot[0];
	    regions(2,1,region_n) = r_top[1];
	    regions(2,2,region_n) = r_bot[1];
	  }
	  //
	  //  Given the number of sectors in the current collar and
	  //  in the next collar, calculate the next offset.
	  //  Accumulate the offset, and force it to be a number between 0 and 1.
	  //  
	  
	  offset += circle_offset(n_in_collar,n_regions[1+collar_n]);
	  offset -= floor(offset);
	} else {
	  for (region_1_n=1; region_1_n <= size_regions_1_3; region_1_n++) {
	    region_n++;
	    
	    //
	    //    Dim-region;
	    //    The first angles are those of the current (dim-1) region of regions_1.
	    //
	    
	    for (j=1; j<=2; j++)
	      for (i=1; i<=dim-1; i++)
	        regions(i,j,region_n) = regions_1(i,j,region_1_n);
	    
	    //
	    //    The last angles are the top and bottom colatitudes of the collar.
	    //   
	    //    regions(dim,:,region_n) = [c_top,c_bot]
	    //
	    
	    regions(dim,1,region_n) = c_top;
	    regions(dim,2,region_n) = c_bot;
	  }
	}

        delete [] regs_1;

      }
      //
      //  End with the bottom cap.
      // 
      //  regions(:,:,N) = bot_cap_region(dim,s_cap(1))
      //
      bot_cap_region(dim,s_cap[0],&regions(1,1,n));
    }
  }
  delete [] s_cap;
  delete [] n_regions;
}

//==============================================================================
double EqRegionCache::eq_area(const double &rn)
//==============================================================================
{
 int n = rn;
  double eq_area = (sphere_area/n);
  return eq_area;
}

//==============================================================================
double EqRegionCache::eq_n(const double &resol) 
//==============================================================================
{
  double dres = ( resol <  min_resol ? min_resol : resol ) * piconst::pi_over_180;
  double eq_area = dres * dres ; /* Actually: (resol*pi/180)^2 * R^2 (approx.) */
  int n = NINT(sphere_area/eq_area); /* Note: R^2's would have been divided away */
  return n;
}

//==============================================================================
double EqRegionCache::get_resol(const double & nval) 
//==============================================================================
{
  return eq_n(nval);
}
//==============================================================================
double EqRegionCache::eq_resol(const double &rn) 
//==============================================================================
{
  double eq_area_value = eq_area(rn);
  double resol = sqrt(eq_area_value) * piconst::recip_pi_over_180; /* In degrees ~ at Equator for small resol's */
  if (resol < min_resol) resol = min_resol;
  return resol;
}

//==============================================================================
void EqRegionCache::create_cache(const double & resol, const int & n) 
//==============================================================================
{
// nothing done for this resolution
  double *regs = NULL;
  int jb, nb = 0;
  int nbelm = dim*2*resol;

  regs = new double [nbelm];
  eq_regions(dim,resol,&regions(1,1,1));

  {
   //   After eq_regions() function call, latitudes & longitudes are in radians
   //   and between [ 0 .. pi ] and [ 0 .. two_pi ], respectively.
   //
   //   Pay special attention to cap regions (j=1 & j=n), where starting and
   //   ending longitude has the same constant value; make them -180 and +180, respectively .

    int j;
    for (j=1; j<=n; j++) {
      // In radians 
      double startlon = regions(1,1,j);
      double startlat = regions(2,1,j);
      double endlon   = regions(1,2,j);
      double endlat   = regions(2,2,j);
      // shift latitudes to start from -half_pi, not 0 
      startlat -= piconst::half_pi;
      endlat -= piconst::half_pi;

      if (j == 1 || j == n) { // cap regions 
        endlon = startlon;
      }

      // Replace values in regions, still in radians 
      regions(1,1,j) = startlon;
      regions(2,1,j) = startlat;
      regions(1,2,j) = endlon;
      regions(2,2,j) = endlat;
    }
  }

    //
    //  Derive how many (nb) latitude bands we have i.e. keep track how often 
    //  the consecutive (startlat,endlat)-pairs change their value.
    //

    int j = 1;
    int ncnt;
    double three_sixty = 360;
    double last_startlat = regions(2,1,j);
    double last_endlat   = regions(2,2,j);

    nb = 1; /* Southern polar cap */
    for (j=2; j<n; j++) { // All but caps 
      double startlat = regions(2,1,j);
      double endlat   = regions(2,2,j);
      if (last_startlat == startlat && last_endlat == endlat) {
        // No change i.e. we are still in the same latitude band 
        continue;
      } else {
        // New latitude band //
        ++nb;
        last_startlat = startlat;
        last_endlat   = endlat;
      }
    } // for (j=2; j<n; j++) 
    if (n > 1) { // Northern polar cap 
      ++nb;
    }

    // 
    // We have now in total "nb" latitude bands.
    // Lets store their starting and ending latitudes for much faster lookup.
    //

    double *latband = new double[nb + 1];
    int *loncnt = new int [nb];
    double *stlon = new double [nb];
    double *deltalon = new double [nb];

    jb = -1;

    j = 1;
    last_startlat = regions(2,1,j);
    last_endlat   = regions(2,2,j);
    loncnt[++jb] = 1;
    latband[jb] = -R2D(last_startlat); // Reverse latitudes from S->N to N->S pole 

    for (j=2; j<n; j++) { // All but caps 
      double startlat = regions(2,1,j);
      double endlat   = regions(2,2,j);
      if (last_startlat == startlat && last_endlat == endlat) {
        // Increase longitude count by one for this latitude band 
        loncnt[jb]++;
      } else {
        // For previous latitude band ... 
        ncnt = loncnt[jb];
        deltalon[jb] = three_sixty/ncnt;
        stlon[jb] = -deltalon[jb]/2; // Starting longitude at 0 GMT minus half longitudinal delta 
        // New latitude band 
        last_startlat = startlat;
        last_endlat   = endlat;
        loncnt[++jb] = 1;
        latband[jb] = -R2D(last_startlat); // Reverse latitudes from S->N to N->S pole 
      }
    } // for (j=2; j<n; j++) 

    if (n > 1) { // Northern polar cap (mapped to be the South pole, in fact) 
      ncnt = loncnt[jb];
      deltalon[jb] = three_sixty/ncnt;
      stlon[jb] = -deltalon[jb]/2; // Starting longitude at 0 GMT minus half longitudinal delta 
      j = n;
      last_startlat = regions(2,1,j);
      last_endlat   = regions(2,2,j);
      loncnt[++jb] = 1;
      latband[jb] = -R2D(last_startlat); // Reverse latitudes from S->N to N->S pole 
    }

    {
      ncnt = loncnt[jb];
      deltalon[jb] = three_sixty/ncnt;
      stlon[jb] = -deltalon[jb]/2; // Starting longitude at 0 GMT minus half longitudinal delta 
      j = n+1;
      latband[++jb] = -R2D(last_endlat); // Reverse latitudes from S->N to N->S pole 
    }

      // Release space allocated by regs 

   delete [] regs;

   // Calculate mid latitudes for each band by a simple averaging process 

   double *midlat = new double [nb];
   for (jb=0; jb<nb; jb++) {
     midlat[jb] = (latband[jb] + latband[jb+1])/2;
   }
   // Store in cache 
   RegionCacheKind kind = eq_cache_kind;
   RegionCache::put_cache(kind, resol, nb, 
                     latband, midlat,
                     stlon, deltalon,
                     loncnt);

}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

