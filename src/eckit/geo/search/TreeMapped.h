// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
