/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Tool.h"
#include "eckit/container/KDTree.h"
#include "eckit/container/KDPoint.h"
#include "eckit/os/Semaphore.h"
#include <list>

using namespace std;
using namespace eckit;

class Test : public Tool {
    virtual void run();

    public:

    Test(int argc, char** argv): Tool(argc,argv) { }

};


void Test::run()
{
    typedef KDPoint<double> ValuePoint;

    KDTree<KDPoint<double> > kd;
    std::vector<ValuePoint> points;

    // test it for single closest point

    for (unsigned int i = 0; i < 10; i++)
        for (unsigned int j = 0; j < 10; j++)
            points.push_back(ValuePoint((double)i, (double)j, 99.9));

    kd.build(points.begin(), points.end());

    // pick some point from the vector
    ValuePoint refPoint = points[points.size() / 2];

    // perturb it a little
    ValuePoint delta(0.1, 0.1, 0.0);
    ValuePoint testPoint = ValuePoint::add(refPoint, delta);
    std::cout << "testPoint perturb " << testPoint.x(0) << ", " << testPoint.x(1) << std::endl;

    ValuePoint nr = kd.nearestNeighbour(testPoint).point();
    
    // we should find the same point
    for (unsigned int i = 0; i < ValuePoint::dimensions(); i++)
        ASSERT(nr.x(i) == refPoint.x(i));


    // test exact match to a point

    nr = kd.nearestNeighbour(refPoint).point();
    for (unsigned int i = 0; i < ValuePoint::dimensions(); i++)
        ASSERT(nr.x(i) == refPoint.x(i));


    // test "off the scale" - i.e. not within a group of points
    delta = ValuePoint(1000.0, 0.0, 0.0);
    // add it to the last point
    testPoint = ValuePoint::add(points.back(), delta);
    nr = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < ValuePoint::dimensions(); i++)
        ASSERT(nr.x(i) == points.back().x(i));

    // and negatively
    //
    delta = ValuePoint(-1000.0, 0.0, 0.0);
    // add it to the first point
    testPoint = ValuePoint::add(points.front(), delta);
    nr = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < ValuePoint::dimensions(); i++)
        ASSERT(nr.x(i) == points.front().x(i));


    // test N nearest
    refPoint = points[points.size() / 2];
    // move this point so it lies between four equally
    delta = ValuePoint(0.5, 0.5, 0.0);
    testPoint = ValuePoint::add(refPoint, delta);

    KDTree<ValuePoint>::NodeList nn = kd.kNearestNeighbours(testPoint, 4);
    for (KDTree<ValuePoint>::NodeList::iterator it = nn.begin(); it != nn.end(); ++it)
    {
        ValuePoint diff = ValuePoint::sub(it->point(), testPoint);
        // make sure we differ by 0.5 along each axis
        for (unsigned int i = 0; i < ValuePoint::dimensions(); ++i)
        {
            std::cout << "distance along point " << ValuePoint::distance(ValuePoint(0.0, 0.0, 0.0), diff, i)  << std::endl;
            ASSERT(ValuePoint::distance(ValuePoint(0.0, 0.0, 0.0), diff, i) == 0.5);
        }

    }


}

//=============================================================

int main(int argc,char **argv)
{
    Test app(argc,argv);
    app.start();
    return 0;
}
