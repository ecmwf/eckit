/*
 * (C) Copyright 1996-2015 ECMWF.
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
#include "eckit/memory/ScopedPtr.h"

#include "eckit/container/KDTree.h"
#include "eckit/container/KDMapped.h"
#include "eckit/container/KDMemory.h"

#include "eckit/geometry/Point2.h"

//------------------------------------------------------------------------------------------------------

struct grib_handle;

namespace eckit {

//------------------------------------------------------------------------------------------------------

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

struct PointIndexTreeTrait
{
    typedef geometry::LLPoint2  Point;
    typedef size_t             Payload;
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
    eckit::ScopedPtr<Tree> tree_;

    std::map<Point,NodeInfo> last_;
    eckit::Mutex mutex_;

};

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
