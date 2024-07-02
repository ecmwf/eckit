/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/geo/Point2.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


//----------------------------------------------------------------------------------------------------------------------

struct TestTreeTrait {
    using Point   = geo::Point2;
    using Payload = double;
};

//----------------------------------------------------------------------------------------------------------------------

/// \brief Class used to test whether any point in a kd-tree lies in the interior of an
/// axis-aligned box.
template <typename TreeTrait>
struct PointInBoxInteriorFinder {
    using KDTree = KDTreeX<TreeTrait>;
    using Point  = typename KDTree::Point;
    using Alloc  = typename KDTree::Alloc;
    using Node   = typename KDTree::Node;

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

        auto& alloc = tree.alloc_;
        auto* root  = alloc.convert(tree.root_, static_cast<Node*>(nullptr));
        ASSERT(root != nullptr);

        return isAnyPointInBoxInterior(root, alloc, lbound, ubound);
    }

private:
    /// \brief Returns true if the point stored in \p node or any of its descendants lies in the
    /// interior of the axis-aligned box with bottom-left and top-right corners at
    /// \p lbound and \p ubound.
    static bool isAnyPointInBoxInterior(const Node* node, Alloc& alloc, const Point& lbound, const Point& ubound) {
        if (node == nullptr) {
            return false;
        }

        const auto& point = node->value().point();

        if (isPointInBoxInterior(point, lbound, ubound)) {
            return true;
        }

        const size_t axis = node->axis();

        return (lbound.x(axis) < point.x(axis) && isAnyPointInBoxInterior(node->left(alloc), alloc, lbound, ubound))
               || (ubound.x(axis) > point.x(axis)
                   && isAnyPointInBoxInterior(node->right(alloc), alloc, lbound, ubound));
    }

    /// \brief Returns true if \p point is in the interior of the axis-aligned box
    /// with bottom-left and top-right corners at \p lbound and \p ubound.
    static bool isPointInBoxInterior(const Point& point, const Point& lbound, const Point& ubound) {
        for (size_t d = 0; d < Point::DIMS; ++d) {
            if (point.x(d) <= lbound.x(d) || point.x(d) >= ubound.x(d)) {
                return false;
            }
        }
        return true;
    }
};

//----------------------------------------------------------------------------------------------------------------------

/// \brief Returns true if any point in \p tree is in the interior of the axis-aligned box
/// with bottom-left and top-right corners at \p lbound and \p ubound.
template <typename TreeTraits>
bool isAnyPointInBoxInterior(const KDTreeX<TreeTraits>& tree, const typename KDTreeX<TreeTraits>::Point& lbound,
                             const typename KDTreeX<TreeTraits>::Point& ubound) {
    return PointInBoxInteriorFinder<TreeTraits>::isAnyPointInBoxInterior(tree, lbound, ubound);
}

//----------------------------------------------------------------------------------------------------------------------

#define EXPECT_POINT_EQUAL(a, b)                       \
    for (size_t i = 0; i < Point::dimensions(); ++i) { \
        EXPECT(a.x(i) == b.x(i));                      \
    }


CASE("test_eckit_container_kdtree_constructor") {
    using Tree  = KDTreeMemory<TestTreeTrait>;
    using Point = Tree::PointType;

    // build k-d tree (offline)
    Tree kd;
    std::vector<Tree::Value> points;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            points.emplace_back(Point{static_cast<double>(i), static_cast<double>(j)}, 99.9);
        }
    }
    kd.build(points.begin(), points.end());

    // size
    EXPECT_EQUAL(kd.size(), points.size());

    // pick a point
    auto ref = points[points.size() / 2].point();

    SECTION("test single closest point") {
        // a point similar to an existing one
        EXPECT_POINT_EQUAL(ref, kd.nearestNeighbour(ref + Point{0.1, 0.1}).point());

        // exact match to a point
        EXPECT_POINT_EQUAL(ref, kd.nearestNeighbour(ref).point());

        // off the scale, i.e. not within a group of points (+)
        EXPECT_POINT_EQUAL(points.back().point(),
                           kd.nearestNeighbour(points.back().point() + Point{1000., 0.}).point());

        // off the scale, i.e. not within a group of points (-)
        EXPECT_POINT_EQUAL(points.front().point(),
                           kd.nearestNeighbour(points.front().point() + Point{-1000., 0.}).point());
    }

    SECTION("test N nearest") {
        // move this point so it lies between four equally, make sure we differ by 0.5 along each axis
        auto test = ref + Point{0.5, 0.5};

        for (auto& near : kd.kNearestNeighbours(test, 4)) {
            auto diff = near.point() - test;
            for (size_t i = 0; i < Point::dimensions(); ++i) {
                EXPECT(Point::distance(Point{0., 0.}, diff, i) == 0.5);
            }
        }
    }

    SECTION("test a custom visitor") {
        // Test a custom visitor. The purpose of doing that in this test is to ensure that the public
        // interface of KDTree is sufficient to write a custom class traversing the tree.
        auto a      = Point{0.25, 0.25};
        auto lbound = ref - a;
        auto ubound = ref + a;
        EXPECT(isAnyPointInBoxInterior(kd, lbound, ubound));

        auto b = Point{0.5, 0.5};
        lbound = lbound + b;
        ubound = ubound + b;
        EXPECT_NOT(isAnyPointInBoxInterior(kd, lbound, ubound));
    }
}

CASE("test_eckit_container_kdtree_insert") {
    using Tree  = KDTreeMemory<TestTreeTrait>;
    using Point = Tree::PointType;

    // build k-d tree (online)
    Tree kd;
    std::vector<Tree::Value> points;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            points.emplace_back(Point{static_cast<double>(i), static_cast<double>(j)}, 99.9);
            kd.insert(points.back());
        }
    }

    // size
    EXPECT_EQUAL(kd.size(), points.size());

    // pick a point
    auto ref = points[points.size() / 2].point();

    SECTION("test single closest point") {
        // a point similar to an existing one
        EXPECT_POINT_EQUAL(ref, kd.nearestNeighbour(ref + Point{0.1, 0.1}).point());

        // exact match to a point
        EXPECT_POINT_EQUAL(ref, kd.nearestNeighbour(ref).point());

        // off the scale, i.e. not within a group of points (+)
        EXPECT_POINT_EQUAL(points.back().point(),
                           kd.nearestNeighbour(points.back().point() + Point{1000., 0.}).point());

        // off the scale, i.e. not within a group of points (-)
        EXPECT_POINT_EQUAL(points.front().point(),
                           kd.nearestNeighbour(points.front().point() + Point{-1000., 0.}).point());
    }

    SECTION("test N nearest") {
        // move this point so it lies between four equally, make sure we differ by 0.5 along each axis
        auto test = ref + Point{0.5, 0.5};

        for (auto& near : kd.kNearestNeighbours(test, 4)) {
            auto diff = near.point() - test;
            for (size_t i = 0; i < Point::dimensions(); ++i) {
                EXPECT(Point::distance(Point{0., 0.}, diff, i) == 0.5);
            }
        }
    }
}

CASE("test_kdtree_mapped") {
    using Tree  = KDTreeMapped<TestTreeTrait>;
    using Point = Tree::PointType;

    std::vector<Tree::Value> points;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            points.emplace_back(Point{static_cast<double>(i), static_cast<double>(j)}, 99.9);
        }
    }

    // pick a point
    auto ref = points[points.size() / 2].point();

    auto passTest = [&](Tree& kd, const Point& p) -> bool {
        // perturb it a little
        // we should find the same point
        auto nr = kd.nearestNeighbour(p + Point{0.1, 0.1}).point();
        for (size_t i = 0; i < Point::dimensions(); ++i) {
            if (nr.x(i) != p.x(i)) {
                return false;
            }
        }
        return true;
    };

    PathName path("test_kdtree_mapped.kdtree");

    // Write file with k-d tree
    {
        if (path.exists()) {
            path.unlink();
        }

        Tree kd(path, points.size(), 0);
        EXPECT(kd.empty());

        kd.build(points);

        EXPECT_EQUAL(kd.size(), points.size());
        EXPECT(passTest(kd, ref));
    }

    // Load file with k-d tree
    {
        Tree kd(path, 0, 0);

        // Cannot insert point as the tree is readonly
        EXPECT_THROWS_AS(kd.insert(points.front()), AssertionFailed);

        // Cannot build with points as the tree is readonly
        EXPECT_THROWS_AS(kd.build(points), AssertionFailed);

        EXPECT_EQUAL(kd.size(), points.size());
        EXPECT(passTest(kd, ref));
    }
}

CASE("test_kdtree_iterate_empty") {
    using Tree = KDTreeMemory<TestTreeTrait>;

    size_t count = 0;
    Tree kd;
    for (auto& item : kd) {
        count++;
    }
    EXPECT_EQUAL(count, 0);
    EXPECT(kd.empty());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
