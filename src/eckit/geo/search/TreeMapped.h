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

#include "eckit/container/KDTree.h"
#include "eckit/geo/search/Tree.h"
#include "eckit/os/AutoUmask.h"


namespace eckit::geo::search {


class TreeMapped : public Tree {
private:

    AutoUmask umask_;  // Must be first
    PathName path_;
    KDTreeMapped<Tree> tree_;

protected:

    PathName path() const { return path_; }

    void build(std::vector<PointValueType>&) override;
    void insert(const PointValueType&) override;
    void statsPrint(std::ostream&, bool pretty) override;
    void statsReset() override;

    PointValueType nearestNeighbour(const Tree::Point&) override;
    std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k) override;
    std::vector<PointValueType> findInSphere(const Point&, double radius) override;

    bool ready() const override              = 0;
    void commit() override                   = 0;
    void print(std::ostream&) const override = 0;

public:

    TreeMapped(const Grid&, const PathName&);
};


}  // namespace eckit::geo::search
