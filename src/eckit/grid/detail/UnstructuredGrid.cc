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


#include "eckit/grid/detail/UnstructuredGrid.h"

#include <fstream>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/grid/Domain.h"
#include "eckit/grid/Iterator.h"
#include "eckit/grid/Projection.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"


namespace eckit::grid::grid {


namespace detail {


class UnstructuredIterator : public Iterator {
public:
    // -- Exceptions
    // None

    // -- Constructors
    UnstructuredIterator(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
        count_(0), size_(latitudes.size()), latitudes_(latitudes), longitudes_(longitudes), first_(true) {
        ASSERT(latitudes_.size() == longitudes_.size());
    }

    UnstructuredIterator(const UnstructuredIterator&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    UnstructuredIterator& operator=(const UnstructuredIterator&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t count_;
    const size_t size_;
    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;
    bool first_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Iterator

    void print(std::ostream& out) const override {
        out << "UnstructuredGridIterator[";
        Iterator::print(out);
        out << "]";
    }

    bool operator++() override {
        if ((first_ ? count_ : ++count_) < size_) {
            first_ = false;
            // lat    = latitudes_[count_];
            // lon    = longitudes_[count_];

            return true;
        }
        return false;
    }

    size_t index() const override { return count_; }

    size_t size() const override { NOTIMP; }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace detail


UnstructuredGrid::UnstructuredGrid(const Configuration& config) :
    Grid(config) {
    config.get("latitudes", latitudes_);
    config.get("longitudes", longitudes_);

    if (latitudes_.empty() || longitudes_.empty()) {
        throw UserError("UnstructuredGrid: requires 'latitudes' and 'longitudes'");
    }
    ASSERT(latitudes_.size() == longitudes_.size());
}


#if 0
UnstructuredGrid::UnstructuredGrid(const PathName& path) {
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw CantOpenFile(path);
    }

    if (::isprint(in.peek()) == 0) {
        IfstreamStream s(in);
        size_t version;
        s >> version;
        ASSERT(version == 1);

        size_t count;
        s >> count;

        latitudes_.resize(count);
        longitudes_.resize(count);

        for (size_t i = 0; i < count; ++i) {
            s >> latitudes_[i];
            s >> longitudes_[i];
        }
    }
    else {
        double lat;
        double lon;
        while (in >> lat >> lon) {
            latitudes_.push_back(lat);
            longitudes_.push_back(lon);
        }
    }
}
#endif


UnstructuredGrid::UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes,
                                   const BoundingBox& bbox) :
    Grid(bbox), latitudes_(latitudes), longitudes_(longitudes) {
    ASSERT(latitudes_.size() == longitudes_.size());
}


UnstructuredGrid::~UnstructuredGrid() = default;


void UnstructuredGrid::print(std::ostream& out) const {
    out << "UnstructuredGrid[points=" << numberOfPoints() << "]";
}


Domain UnstructuredGrid::domain() const {
    // FIXME Should be global?
    return {bbox().north(), bbox().west(), bbox().south(), bbox().east()};
}


size_t UnstructuredGrid::numberOfPoints() const {
    ASSERT(latitudes_.size() == longitudes_.size());
    return latitudes_.size();
}


Renumber UnstructuredGrid::crop(BoundingBox&) const {
    NOTIMP;
}


Renumber UnstructuredGrid::reorder(long scanningMode) const {
    NOTIMP;
}


Iterator* UnstructuredGrid::iterator() const {
    return new detail::UnstructuredIterator(latitudes_, longitudes_);
}


bool UnstructuredGrid::isPeriodicWestEast() const {
    return bbox().east() - bbox().west() == GLOBE;
}


bool UnstructuredGrid::includesNorthPole() const {
    return bbox().north() == NORTH_POLE;
}


bool UnstructuredGrid::includesSouthPole() const {
    return bbox().south() == SOUTH_POLE;
}


static const GridBuilder<UnstructuredGrid> triangular_grid("triangular_grid");
static const GridBuilder<UnstructuredGrid> unstructured_grid("unstructured_grid");


}  // namespace eckit::grid::grid
