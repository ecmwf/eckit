/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef PointIndex_H
#define PointIndex_H

#include <cmath>

#include "eckit/eckit.h"
#include "eckit/filesystem/PathName.h"

#include "eckit/grib/Grib.h"

#include "eckit/container/KDTree.h"
#include "eckit/container/KDMapped.h"
#include "eckit/container/KDMemory.h"
#include "eckit/container/sptree/SPPoint.h"

using eckit::SPPoint;

const double earthRadius = 6367.47; // In ECMWF model...

class LLPoint0 : public SPPoint<2> {


public:

    double lat() const { return x(0); }
    double lon() const { return x(1); }

    LLPoint0():
        SPPoint<2>(0,0) {}

    LLPoint0(double lat, double lon):
        SPPoint<2>(lat,lon) {}

//    static double km(const LLPoint0& a, const LLPoint0& b)
//    {
//        return ::grib_nearest_distance(earthRadius, a.lon() , a.lat() , b.lon(), b.lat());
//    }

};

class LLPoint1 : public SPPoint<2> {


public:

    double lat() const { return x(0); }
    double lon() const { return x(1); }

    LLPoint1():
        SPPoint<2>(0,0) {}

    LLPoint1(double lat, double lon):
        SPPoint<2>(lat,lon) {}

//    static double km(const LLPoint1& a, const LLPoint1& b)
//    {
//        return ::grib_nearest_distance(earthRadius, a.lon() , a.lat() , b.lon(), b.lat());
//    }

    /* This distance does not quite work with kd-tree. kd-tree needs an euclidand distance */
    static double distance(const LLPoint1& a, const LLPoint1& b)
    {
        return ::grib_nearest_distance(earthRadius, a.lon(), a.lat() , b.lon(), b.lat());
    }

    static double distance(const LLPoint1& a, const LLPoint1& b, int axis)
    {
        if(axis) { // longitude
            double lat = a.lat(); //(a.lat() + b.lat()) / 2;
            return ::grib_nearest_distance(earthRadius,a.lon() , lat , b.lon(), lat);
        }
        else {
            // latitude
            double lon = a.lon(); //(a.lon() + b.lon()) / 2;
            return ::grib_nearest_distance(earthRadius, lon , a.lat() , lon, b.lat());
        }
    }
};

class LLPoint2 : public SPPoint<3> {


    double lat_;
    double lon_;

public:

    LLPoint2(): SPPoint<3>() {}

    double lat() const { return lat_; }
    double lon() const { return lon_; }

    LLPoint2(double lat, double lon):
        SPPoint<3>(),
        lat_(lat), lon_(lon)
    {
        // See http://en.wikipedia.org/wiki/Geodetic_system#From_geodetic_to_ECEF
        double& X = x_[0];
        double& Y = x_[1];
        double& Z = x_[2];

        double h = 0; // Altitude

        double a = earthRadius; //6378137.0 ; //  WGS84 semi-major axis
        double e2 = 0;          //6.69437999014E-3; // WGS84 first numerical eccentricity sqared

        double phi = lat / 180.0 * M_PI;
        double lambda = lon / 180.0 * M_PI;

        double cos_phi = cos(phi);
        double sin_phi = sin(phi);
        double cos_lambda = cos(lambda);
        double sin_lambda = sin(lambda);

        double N_phi = a/sqrt(1-e2*sin_phi*sin_phi);

        X = (N_phi + h) * cos_phi * cos_lambda;
        Y = (N_phi + h) * cos_phi * sin_lambda;
        Z = (N_phi * (1-e2) + h) * sin_phi;
    }

    friend std::ostream& operator<<(std::ostream& s,const LLPoint2& p)
    {
        s << '(' << p.lat_ << "," << p.lon_ << ')';
        return s;
    }

//    static double km(const LLPoint2& a, const LLPoint2& b)
//    {
//        return ::grib_nearest_distance(earthRadius, a.lon() , a.lat() , b.lon(), b.lat());
//    }

};

template<class Traits>
class LLTree : public eckit::KDTreeMapped<Traits> {
public:
    typedef eckit::KDTreeMapped<Traits> Tree;
    typedef typename Tree::Point Point;
    typedef typename Tree::Alloc    Alloc;

    typedef typename Tree::NodeList    NodeList;
    typedef typename Tree::NodeInfo    NodeInfo;
    typedef typename Tree::PayloadType Payload;
    typedef typename Tree::Value   Value;

    LLTree(const eckit::PathName& path,  size_t itemCount, size_t metadataSize):
        eckit::KDTreeMapped<Traits>(path, itemCount, metadataSize) {}

    NodeInfo nearestNeighbour(const Point& p)
    {
        return Tree::nearestNeighbour(p);
    }

    NodeInfo nearestNeighbourBruteForce(const Point& p)
    {
        return Tree::nearestNeighbourBruteForce(p);
    }

    NodeList findInSphere(const Point& p,double radius)
    {
        return Tree::findInSphere(p, radius);
    }

    NodeList findInSphereBruteForce(const Point& p,double radius)
    {
        return Tree::findInSphereBruteForce(p,radius);
    }

    NodeList kNearestNeighbours(const Point& p,size_t k)
    {
        return Tree::kNearestNeighbours(p, k);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p,size_t k)
    {
        return Tree::kNearestNeighboursBruteForce(p,k);
    }

    LLTree(Alloc& alloc):
        Tree(alloc) {}
};

struct PointIndexTreeTrait {
    typedef LLPoint2   Point;
    typedef size_t     Payload;
};

class PointIndex {
public:

    typedef LLTree<PointIndexTreeTrait>  Tree;
    typedef Tree::Point       Point;
    typedef Tree::Payload     Payload;
    typedef Tree::NodeInfo    NodeInfo;

    NodeInfo nearestNeighbour(double lat, double lon);

    static PointIndex& lookUp(const std::string& md5);
    static std::string cache(grib_handle*);

private:

    PointIndex(const eckit::PathName&, Tree* tree = 0);
    ~PointIndex();

    eckit::PathName path_;
    std::auto_ptr<Tree> tree_;

    std::map<Point,NodeInfo> last_;
    eckit::Mutex mutex_;

};

#endif
