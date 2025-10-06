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


#pragma once

#include <array>
#include <ostream>
#include <vector>

#include "eckit/container/sptree/SPValue.h"
#include "eckit/geo/PointXYZ.h"


namespace eckit::geo {
class Grid;
}


namespace eckit::geo::search {


class Tree {
public:

#if 0
    using Point = PointXYZ;
#else
    struct Point : private std::array<double, 3> {
        static constexpr size_t DIMS = 3;

        Point(value_type x, value_type y, value_type z) : array{x, y, z} {}
        using array::array;

        explicit Point(const PointXYZ& p) : Point{p.X, p.Y, p.Z} {}

        value_type x(size_t axis) const { return operator[](axis); }

        PointXYZ to_xyz() const { return {operator[](0), operator[](1), operator[](2)}; }  // (additional)

        static value_type distance(const Point& p, const Point& q);
        static value_type distance(const Point& p, const Point& q, size_t axis);

        static constexpr value_type EPS = PointXYZ::EPS;

        friend std::ostream& operator<<(std::ostream& out, const Point& p) {
            return out << '{' << p[0] << ", " << p[1] << ", " << p[2] << '}';
        }
    };
#endif

    using Payload        = size_t;
    using PointValueType = SPValue<Tree>;

    explicit Tree(const Grid&);

    Tree(const Tree&) = delete;
    Tree(Tree&&)      = delete;

    virtual ~Tree();

    Tree& operator=(const Tree&) = delete;
    Tree& operator=(Tree&&)      = delete;

    virtual void build(std::vector<PointValueType>&);

    virtual void insert(const PointValueType&);
    virtual void statsPrint(std::ostream&, bool);
    virtual void statsReset();

    virtual PointValueType nearestNeighbour(const Point&);
    virtual std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k);
    virtual std::vector<PointValueType> findInSphere(const Point&, double);

    virtual bool ready() const;
    virtual void commit();
    virtual void print(std::ostream&) const;

    virtual void lock();
    virtual void unlock();

    std::string str() const;

    size_t itemCount() const { return itemCount_; }

    friend std::ostream& operator<<(std::ostream& s, const Tree& p) {
        p.print(s);
        return s;
    }

private:

    const size_t itemCount_;
};


class TreeFactory {
protected:

    virtual Tree* make(const Grid&) = 0;
    explicit TreeFactory(const std::string&);
    virtual ~TreeFactory();

public:

    TreeFactory(const TreeFactory&) = delete;
    TreeFactory(TreeFactory&&)      = delete;

    TreeFactory& operator=(TreeFactory&&)      = delete;
    TreeFactory& operator=(const TreeFactory&) = delete;

    static Tree* build(const std::string&, const Grid&);
    static void list(std::ostream&);

private:

    std::string name_;
};


template <class T>
class TreeBuilder : public TreeFactory {
    Tree* make(const Grid& r) override { return new T(r); }

public:

    explicit TreeBuilder(const std::string& name) : TreeFactory(name) {}
};


}  // namespace eckit::geo::search
