/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <list>

#include "ecbuild/boost_test_framework.h"

#include "eckit/container/KDTree.h"
#include "eckit/geometry/Point2.h"
#include "eckit/os/Semaphore.h"

using namespace std;
using namespace eckit;
using namespace eckit::geometry;

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct TestTreeTrait {
    typedef Point2   Point;
    typedef double   Payload;
};

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_container_kdtree )

BOOST_AUTO_TEST_CASE( test_eckit_container_kdtree_constructor )
{
    typedef KDTreeMemory<TestTreeTrait> Tree;

    Tree kd;
    typedef Tree::PointType Point;

    std::vector<Tree::Value> points;

    // test it for single closest point

    for (unsigned int i = 0; i < 10; i++) {
        for (unsigned int j = 0; j < 10; j++) {
            Point p = Point(double(i), double(j));
            Tree::Value v(p, 99.9);
            points.push_back(v);
        }
    }

    kd.build(points.begin(), points.end());

    // pick some point from the vector
    Point refPoint = points[points.size() / 2].point();

    // perturb it a little
    Point delta(0.1, 0.1);
    Point testPoint = Point::add(refPoint, delta);
    std::cout << "testPoint perturb " << testPoint.x(0) << ", " << testPoint.x(1) << std::endl;

    Point nr = kd.nearestNeighbour(testPoint).point();

    // we should find the same point
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        BOOST_CHECK_EQUAL(nr.x(i) , refPoint.x(i));


    // test exact match to a point

    nr = kd.nearestNeighbour(refPoint).point();
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        BOOST_CHECK_EQUAL(nr.x(i) , refPoint.x(i));


    // test "off the scale" - i.e. not within a group of points
    delta = Point(1000.0, 0.0);
    // add it to the last point
    testPoint = Point::add(points.back().point(), delta);
    nr = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        BOOST_CHECK_EQUAL(nr.x(i) , points.back().point().x(i));

    // and negatively
    //
    delta = Point(-1000.0, 0.0);
    // add it to the point() point
    testPoint = Point::add(points.front().point(), delta);
    nr = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        BOOST_CHECK_EQUAL(nr.x(i) , points.front().point().x(i));


    // test N nearest
    refPoint = points[points.size() / 2].point();
    // move this point so it lies between four equally
    delta = Point(0.5, 0.5);
    testPoint = Point::add(refPoint, delta);

    Tree::NodeList nn = kd.kNearestNeighbours(testPoint, 4);
    for (Tree::NodeList::iterator it = nn.begin(); it != nn.end(); ++it)
    {
        Point diff = Point::sub(it->point(), testPoint);
        // make sure we differ by 0.5 along each axis
        for (unsigned int i = 0; i < Point::dimensions(); ++i)
        {
            std::cout << "distance along point " << Point::distance(Point(0.0, 0.0), diff, i)  << std::endl;
            BOOST_CHECK_EQUAL(Point::distance(Point(0.0, 0.0), diff, i) , 0.5);
        }

    }

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
