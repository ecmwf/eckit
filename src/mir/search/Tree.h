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

#include <ostream>
#include <vector>

#include "eckit/container/sptree/SPValue.h"
#include "eckit/geometry/Point3.h"

#include "mir/util/Types.h"


namespace mir::repres {
class Representation;
}  // namespace mir::repres


namespace mir::search {


class Tree {
    Tree(const Tree&)            = delete;
    Tree& operator=(const Tree&) = delete;

public:
    using Point          = Point3;
    using Payload        = size_t;
    using PointValueType = eckit::SPValue<Tree>;

public:
    Tree(const repres::Representation&);
    virtual ~Tree();

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

    size_t itemCount() const { return itemCount_; }

    friend std::ostream& operator<<(std::ostream& s, const Tree& p) {
        p.print(s);
        return s;
    }

private:
    const size_t itemCount_;
};

class TreeFactory {
    TreeFactory(const TreeFactory&)            = delete;
    TreeFactory& operator=(const TreeFactory&) = delete;

protected:
    std::string name_;
    virtual Tree* make(const repres::Representation&) = 0;
    TreeFactory(const std::string&);
    virtual ~TreeFactory();

public:
    static Tree* build(const std::string&, const repres::Representation&);
    static void list(std::ostream&);
};

template <class T>
class TreeBuilder : public TreeFactory {
    Tree* make(const repres::Representation& r) override { return new T(r); }

public:
    TreeBuilder(const std::string& name) : TreeFactory(name) {}
};


}  // namespace mir::search
