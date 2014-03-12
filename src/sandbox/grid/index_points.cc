#include <limits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

#define EIGEN_NO_AUTOMATIC_RESIZING
#define EIGEN_DONT_ALIGN
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include "grib_api.h"

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"
#include "atlas/MeshGen.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Field.hpp"
#include "atlas/Parameters.hpp"

#include "PointIndex3.h"

//------------------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//------------------------------------------------------------------------------------------------------

using namespace Eigen;
using namespace atlas;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

std::vector< Point3 >* read_ll_points_from_grib( const std::string& filename )
{
    int err = 0;

    // points to read

    std::vector< Point3 >* pts = new std::vector< Point3 >();

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        while(lon < 0)    lon += 360;
        while(lon >= 360) lon -= 360;

        pts->push_back( Point3() );

        atlas::latlon_to_3d( lat, lon, pts->back().x );
    }
    grib_iterator_delete(i);

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

    #if 0
    for( int e = 0; e < pts->size(); ++e )
    {
        const Point3& p = (*pts)[e];
        std::cout <<  p(XX) << " "
                  <<  p(YY) << " "
                  <<  p(ZZ) << std::endl;
    }
    #endif

    return pts;
}

//------------------------------------------------------------------------------------------------------

FieldT<double>& read_field_from_grib( const std::string& filename, atlas::Mesh* mesh )
{
    FunctionSpace& nodes  = mesh->function_space( "nodes" );

    const size_t nb_nodes = nodes.bounds()[1];

    FieldT<double>& field = nodes.create_field<double>("field",1);

    int err = 0;

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    size_t in = 0;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        if( in >= nb_nodes )
            throw SeriousBug( "field is of incorrect size" );
        field[in] = value;
        ++in;
    }
    grib_iterator_delete(i);

    if( in != nb_nodes )
        throw SeriousBug( "field is of incorrect size" );

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

    return field;
}

//------------------------------------------------------------------------------------------------------

#define NLATS 1000
#define NLONG 1000

#define KN 4

//------------------------------------------------------------------------------------------------------

template < int DIM, typename VT1, typename VT2 >
double distance2( const VT1& v1, const VT2& v2 )
{
    double d = 0.;
    for( int i = 0; i < DIM; i++ )
    {
        double d_ =(v1[i] - v2[i]);
        d += d_*d_;
    }
    return d;
}

//------------------------------------------------------------------------------------------------------

// See http://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
//  for the potential portability problems with the union and bit-fields below.

union Float_t
{
    Float_t(float num = 0.0f) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return (i >> 31) != 0; }
    int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
    int32_t RawExponent() const { return (i >> 23) & 0xFF; }

    int32_t i;
    float f;
};

union Double_t
{
    Double_t( double num = 0.0 ) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return (i >> 31) != 0; }
    int64_t RawMantissa() const { return i & ((1 << 23) - 1); }
    int64_t RawExponent() const { return (i >> 23) & 0xFF; }

    int64_t i;
    double  f;
};

bool AlmostEqualUlps(float A, float B, int maxUlpsDiff)
{
    Float_t uA(A);
    Float_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
    {
        // Check for equality to make sure +0==-0
        if (A == B)
            return true;
        return false;
    }

    // Find the difference in ULPs.
    int ulpsDiff = abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualUlps(double A, double B, int maxUlpsDiff)
{
    Double_t uA(A);
    Double_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
    {
        // Check for equality to make sure +0==-0
        if (A == B)
            return true;
        return false;
    }

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualUlpsAndAbs(float A, float B, float maxDiff, int maxUlpsDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float absDiff = fabs(A - B);
    if (absDiff <= maxDiff)
        return true;

    Float_t uA(A);
    Float_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
        return false;

    // Find the difference in ULPs.
    int ulpsDiff = abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualRelativeAndAbs(float A, float B, float maxDiff, float maxRelDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float diff = fabs(A - B);
    if (diff <= maxDiff)
        return true;

    A = fabs(A);
    B = fabs(B);
    float largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}

bool AlmostEqualUlpsAndAbs(double A, double B, double maxDiff, int maxUlpsDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    double absDiff = std::abs(A - B);
    if (absDiff <= maxDiff)
        return true;

    Double_t uA(A);
    Double_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
        return false;

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool almost_equal( double A, double B )
{
//    return AlmostEqualUlps(A,B,10);
    return AlmostEqualUlpsAndAbs(A,B,std::numeric_limits<double>::epsilon(),10);
}

//------------------------------------------------------------------------------------------------------

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::scientific;

    std::vector< Point3 >* opts = atlas::MeshGen::generate_latlon_points(NLATS, NLONG);
    const size_t npts = opts->size();

    /// BUILD KD-TREE ////////////////////////////////////////////////////////////

    std::vector< PointIndex3::Value > pidx;
    pidx.reserve(npts);

    for( size_t ip = 0; ip < npts; ++ip )
    {
        Point3& p = (*opts)[ip];

        pidx.push_back( PointIndex3::Value( PointIndex3::Point(p), ip ) );
    }

    PathName path (std::string("~/tmp/cache/grid/") + "test" + ".kdtree");
    PathName tmp  (std::string("~/tmp/cache/grid/") + "test" + ".tmp");
    tmp.unlink();

    PointIndex3* tree = new PointIndex3(tmp, npts, 0);

    tree->build(pidx.begin(), pidx.end());

    PathName::rename(tmp, path);

    /// SEARCH ////////////////////////////////////////////////////////////

    for( size_t ip = 0; ip < opts->size(); ++ip )
    {
        KPoint3 p (  (*opts)[ip] );
//        std::cout << "point " << ip << " - " << p << std::endl;

        PointIndex3::NodeList nearest = tree->kNearestNeighbours(p,KN);

        bool found = false;
        for( size_t i = 0; i < nearest.size(); ++i )
        {
            double d2 = distance2<3>( nearest[i].value().point(), p.data() );

            if( almost_equal(d2,0.0) ) { found = true; break; }
        }

        if(!found)
        {
            std::cout << "point not found:" << ip << " - " << p << std::endl;
            exit(-1);
        }
    }

    delete opts;
    delete tree;

    return 0;
}
