/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <list>

#include "eckit/container/KDTree.h"
#include "eckit/geometry/Point2.h"
#include "eckit/os/Semaphore.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::geometry;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

struct TestTreeTrait {
    typedef Point2 Point;
    typedef double Payload;
};

//----------------------------------------------------------------------------------------------------------------------

/// \brief Class used to test whether any point in a kd-tree lies in the interior of an
/// axis-aligned box.
template <typename TreeTrait>
class PointInBoxInteriorFinder {
public:
    typedef eckit::KDTreeX<TreeTrait> KDTree;
    typedef typename KDTree::Point Point;

private:
    typedef typename KDTree::Alloc Alloc;
    typedef typename KDTree::Node Node;

public:
    /// \brief Returns true if any point in \p tree lies in the interior of the specified
    /// axis-aligned box.
    ///
    /// \param tree
    ///   Tree to search.
    /// \param lbound
    ///   Lower-left corner of the axis-aligned box.
    /// \param ubound
    ///   Upper-right corner of the axis-aligned box.
    static bool isAnyPointInBoxInterior(const KDTree& tree, const Point& lbound, const Point& ubound) {
        if (!tree.root_) {
            return false;
        }
        Alloc& alloc = tree.alloc_;
        Node* root   = alloc.convert(tree.root_, static_cast<Node*>(nullptr));

        return isAnyPointInBoxInterior(root, alloc, lbound, ubound);
    }

private:
    /// \brief Returns true if the point stored in \p node or any of its descendants lies in the
    /// interior of the axis-aligned box with bottom-left and top-right corners at
    /// \p lbound and \p ubound.
    static bool isAnyPointInBoxInterior(const Node* node, Alloc& alloc, const Point& lbound, const Point& ubound) {
        const Point& point = node->value().point();

        if (isPointInBoxInterior(point, lbound, ubound))
            return true;

        const size_t axis = node->axis();

        if (lbound.x(axis) < point.x(axis))
            if (Node* left = node->left(alloc))
                if (isAnyPointInBoxInterior(left, alloc, lbound, ubound))
                    return true;

        if (ubound.x(axis) > point.x(axis))
            if (Node* right = node->right(alloc))
                if (isAnyPointInBoxInterior(right, alloc, lbound, ubound))
                    return true;

        return false;
    }

    /// \brief Returns true if \p point is in the interior of the axis-aligned box
    /// with bottom-left and top-right corners at \p lbound and \p ubound.
    static bool isPointInBoxInterior(const Point& point, const Point& lbound, const Point& ubound) {
        for (size_t d = 0; d < Point::DIMS; ++d) {
            if (point.x(d) <= lbound.x(d) || point.x(d) >= ubound.x(d))
                return false;
        }
        return true;
    }
};

//----------------------------------------------------------------------------------------------------------------------

/// \brief Returns true if any point in \p tree is in the interior of the axis-aligned box
/// with bottom-left and top-right corners at \p lbound and \p ubound.
template <typename TreeTraits>
bool isAnyPointInBoxInterior(const eckit::KDTreeX<TreeTraits>& tree,
                             const typename eckit::KDTreeX<TreeTraits>::Point& lbound,
                             const typename eckit::KDTreeX<TreeTraits>::Point& ubound) {
    return PointInBoxInteriorFinder<TreeTraits>::isAnyPointInBoxInterior(tree, lbound, ubound);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_container_kdtree_constructor") {
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
    Log::info() << "testPoint perturb " << testPoint.x(0) << ", " << testPoint.x(1) << std::endl;

    Point nr = kd.nearestNeighbour(testPoint).point();

    // we should find the same point
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == refPoint.x(i));


    // test exact match to a point

    nr = kd.nearestNeighbour(refPoint).point();
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == refPoint.x(i));


    // test "off the scale" - i.e. not within a group of points
    delta = Point(1000.0, 0.0);
    // add it to the last point
    testPoint = Point::add(points.back().point(), delta);
    nr        = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == points.back().point().x(i));

    // and negatively
    //
    delta = Point(-1000.0, 0.0);
    // add it to the point() point
    testPoint = Point::add(points.front().point(), delta);
    nr        = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == points.front().point().x(i));


    // test N nearest
    refPoint = points[points.size() / 2].point();
    // move this point so it lies between four equally
    delta     = Point(0.5, 0.5);
    testPoint = Point::add(refPoint, delta);

    Tree::NodeList nn = kd.kNearestNeighbours(testPoint, 4);
    for (Tree::NodeList::iterator it = nn.begin(); it != nn.end(); ++it) {
        Point diff = Point::sub(it->point(), testPoint);
        // make sure we differ by 0.5 along each axis
        for (unsigned int i = 0; i < Point::dimensions(); ++i) {
            Log::info() << "distance along point " << Point::distance(Point(0.0, 0.0), diff, i) << std::endl;
            EXPECT(Point::distance(Point(0.0, 0.0), diff, i) == 0.5);
        }
    }

    // Test a custom visitor. The purpose of doing that in this test is to ensure that the public
    // interface of KDTree is sufficient to write a custom class traversing the tree.
    delta        = Point(0.25, 0.25);
    Point lbound = Point::sub(refPoint, delta);
    Point ubound = Point::add(refPoint, delta);
    EXPECT(isAnyPointInBoxInterior(kd, lbound, ubound));

    delta  = Point(0.5, 0.5);
    lbound = Point::add(lbound, delta);
    ubound = Point::add(ubound, delta);
    EXPECT_NOT(isAnyPointInBoxInterior(kd, lbound, ubound));

    // Test size()
    EXPECT_EQUAL(kd.size(), points.size());
}

CASE("test_eckit_container_kdtree_insert") {
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
            kd.insert(v);
        }
    }


    // pick some point from the vector
    Point refPoint = points[points.size() / 2].point();

    // perturb it a little
    Point delta(0.1, 0.1);
    Point testPoint = Point::add(refPoint, delta);
    Log::info() << "testPoint perturb " << testPoint.x(0) << ", " << testPoint.x(1) << std::endl;

    Point nr = kd.nearestNeighbour(testPoint).point();

    // we should find the same point
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == refPoint.x(i));


    // test exact match to a point

    nr = kd.nearestNeighbour(refPoint).point();
    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == refPoint.x(i));


    // test "off the scale" - i.e. not within a group of points
    delta = Point(1000.0, 0.0);
    // add it to the last point
    testPoint = Point::add(points.back().point(), delta);
    nr        = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == points.back().point().x(i));

    // and negatively
    //
    delta = Point(-1000.0, 0.0);
    // add it to the point() point
    testPoint = Point::add(points.front().point(), delta);
    nr        = kd.nearestNeighbour(testPoint).point();

    for (unsigned int i = 0; i < Point::dimensions(); i++)
        EXPECT(nr.x(i) == points.front().point().x(i));


    // test N nearest
    refPoint = points[points.size() / 2].point();
    // move this point so it lies between four equally
    delta     = Point(0.5, 0.5);
    testPoint = Point::add(refPoint, delta);

    Tree::NodeList nn = kd.kNearestNeighbours(testPoint, 4);
    for (Tree::NodeList::iterator it = nn.begin(); it != nn.end(); ++it) {
        Point diff = Point::sub(it->point(), testPoint);
        // make sure we differ by 0.5 along each axis
        for (unsigned int i = 0; i < Point::dimensions(); ++i) {
            Log::info() << "distance along point " << Point::distance(Point(0.0, 0.0), diff, i) << std::endl;
            EXPECT(Point::distance(Point(0.0, 0.0), diff, i) == 0.5);
        }
    }
}

CASE("test_kdtree_mapped") {
    using Tree  = KDTreeMapped<TestTreeTrait>;
    using Point = Tree::PointType;
    std::vector<Tree::Value> points;
    for (unsigned int i = 0; i < 10; i++) {
        for (unsigned int j = 0; j < 10; j++) {
            Point p = Point(double(i), double(j));
            Tree::Value v(p, 99.9);
            points.push_back(v);
        }
    }
    auto passTest = [&](Tree& kd) -> bool {
        // pick some point from the vector
        Point refPoint = points[points.size() / 2].point();

        // perturb it a little
        Point delta(0.1, 0.1);
        Point testPoint = Point::add(refPoint, delta);

        Point nr = kd.nearestNeighbour(testPoint).point();

        // we should find the same point
        for (unsigned int i = 0; i < Point::dimensions(); i++) {
            if (nr.x(i) != refPoint.x(i)) {
                return false;
            }
        }
        return true;
    };

    eckit::PathName path("test_kdtree_mapped.kdtree");

    // Write file with kdtree
    {
        if (path.exists())
            path.unlink();
        Tree kd(path, points.size(), 0);
        EXPECT_EQUAL(kd.size(), 0);
        kd.build(points);
        EXPECT_EQUAL(kd.size(), points.size());
        EXPECT(passTest(kd));
    }

    // Load file with kdtree
    {
        Tree kd(path, 0, 0);

        // Cannot insert point as the tree is readonly
        EXPECT_THROWS_AS(kd.insert(points.front()), eckit::AssertionFailed);

        // Cannot build with points as the tree is readonly
        EXPECT_THROWS_AS(kd.build(points), eckit::AssertionFailed);

        EXPECT_EQUAL(kd.size(), points.size());
        EXPECT(passTest(kd));
    }
}

CASE("test_kdtree_iterate_empty") {
    using Tree = KDTreeMemory<TestTreeTrait>;

    Tree kd;
    size_t count{0};
    for (auto& item : kd) {
        count++;
    }
    EXPECT_EQUAL(count, 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
