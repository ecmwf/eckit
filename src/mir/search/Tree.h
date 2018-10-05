/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @author Peter Bispham
/// @date Apr 2015


#ifndef mir_search_Tree_h
#define mir_search_Tree_h

#include <iostream>
#include <vector>

#include "eckit/container/sptree/SPValue.h"
#include "eckit/geometry/Point3.h"


namespace mir {
namespace param {
class MIRParametrisation;
};
namespace repres {
class Representation;
}
}


namespace mir {
namespace search {


class Tree {
public:
    using Point = eckit::geometry::Point3;
    using Payload = size_t;
    using PointValueType = eckit::SPValue<Tree>;

public:
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

    friend std::ostream& operator<<(std::ostream& s, const Tree& p) {
        p.print(s);
        return s;
    }
};

class TreeFactory {

    std::string name_;

    virtual Tree* make(const repres::Representation& r, const param::MIRParametrisation&, size_t itemCount) = 0;

protected:
    TreeFactory(const std::string&);

    virtual ~TreeFactory();

public:
    static Tree* build(const std::string&, const repres::Representation& r, const param::MIRParametrisation&,
                       size_t itemCount);

    static void list(std::ostream&);
};

template <class T> class TreeBuilder : public TreeFactory {
    virtual Tree* make(const repres::Representation& r, const param::MIRParametrisation& param, size_t itemCount) {
        return new T(r, param, itemCount);
    }

public:
    TreeBuilder(const std::string& name) : TreeFactory(name) {}
};


}  // namespace search
}  // namespace mir


#endif
