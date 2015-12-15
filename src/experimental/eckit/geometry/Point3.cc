#include "eckit/geometry/Point3.h"
#include "eckit/types/FloatCompare.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

void Point3::print(std::ostream &s) const { s << "Point3["
                                              << x_[XX] << ","
                                              << x_[YY] << ","
                                              << x_[ZZ] << "]"; }

std::ostream& operator<<(std::ostream& s,const Point3& p)
{
    p.print(s); return s;
}

void LLPoint3::print(std::ostream &s) const { s << "LLPoint3["
                                                << "lat=" << lat_ << ","
                                                << "lon=" << lon_ << "]"; }

std::ostream& operator<<(std::ostream& s,const LLPoint3& p)
{
    p.print(s); return s;
}

const double radius_earth = 6371229; // from ECMWF model ...

    // 6371229  -- IFS
    // 6367470  -- GRIB1
    // 6378137  -- WGS84 semi-major axis

void latlon_to_3d(const double lat, const double lon, double xyz[], const double r, const double h )
{
    // See http://en.wikipedia.org/wiki/Geodetic_system#From_geodetic_to_ECEF

    double& X = xyz[XX];
    double& Y = xyz[YY];
    double& Z = xyz[ZZ];

    const double a = r;
    const double e2 = 0;  // ignored -- 6.69437999014E-3; // WGS84 first numerical eccentricity squared

    const double phi = lat / 180.0 * M_PI;
    const double lambda = lon / 180.0 * M_PI;

    const double cos_phi = cos(phi);
    const double sin_phi = sin(phi);
    const double cos_lambda = cos(lambda);
    const double sin_lambda = sin(lambda);

    const double N_phi = a/sqrt(1-e2*sin_phi*sin_phi);

    X = (N_phi + h) * cos_phi * cos_lambda;
    Y = (N_phi + h) * cos_phi * sin_lambda;
    Z = (N_phi * (1-e2) + h) * sin_phi;
}

void latlon_to_3d( const double lat, const double lon, double xyz[] )
{
    latlon_to_3d( lat, lon, xyz, radius_earth, 0. );
}

void lonlat_to_3d( const double lon, const double lat, double xyz[] )
{
    latlon_to_3d( lat, lon, xyz, radius_earth, 0. );
}

void lonlat_to_3d( const double lonlat[], double xyz[] )
{
  latlon_to_3d(lonlat[LAT],lonlat[LON],xyz);
}

void lonlat_to_3d(const double lon, const double lat, double xyz[], const double r, const double h )
{
  latlon_to_3d(lat, lon, xyz, r, h);
}

void lonlat_to_3d( const double lonlat[], double xyz[], const double r, const double h )
{
  latlon_to_3d(lonlat[LAT],lonlat[LON],xyz,r,h);
}

bool points_equal(const Point3 &a, const Point3 &b)
{
    return FloatCompare<double>::isApproximatelyEqual( Point3::distance2(a,b), 0.0 );
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

