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


#include "eckit/geo/grid/UnstructuredGrid.h"

#include <fstream>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Iterator.h"
#include "eckit/geo/grid/ORCA.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"


namespace eckit::geo::grid {


UnstructuredGrid::UnstructuredGrid(const Configuration& config) {
    config.get("latitudes", latitudes_);
    config.get("longitudes", longitudes_);

    if (latitudes_.empty() || longitudes_.empty()) {
        throw UserError("UnstructuredGrid: requires 'latitudes' and 'longitudes'");
    }
    ASSERT(latitudes_.size() == longitudes_.size());

    check_duplicate_points("UnstructuredGrid from Configuration", latitudes_, longitudes_, config);
}


UnstructuredGrid::UnstructuredGrid(const PathName& path) {
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw CantOpenFile(path);
    }

    if (::isprint(in.peek()) == 0) {

        Log::info() << "UnstructuredGrid::load  " << path << std::endl;

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
            // Log::info() << latitudes_[i] << " " << longitudes_[i] << std::endl;
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

    check_duplicate_points("UnstructuredGrid from " + path.asString(), latitudes_, longitudes_);
}


void UnstructuredGrid::save(const PathName& path, const std::vector<double>& latitudes,
                            const std::vector<double>& longitudes, bool binary) {
    Log::info() << "UnstructuredGrid::save " << path << std::endl;

    check_duplicate_points("UnstructuredGrid::save to " + path.asString(), latitudes, longitudes);

    ASSERT(latitudes.size() == longitudes.size());
    if (binary) {
        FileStream s(path, "w");
        size_t version = 1;
        size_t count   = latitudes.size();
        s << version;
        s << count;
        for (size_t i = 0; i < count; ++i) {
            s << latitudes[i];
            s << longitudes[i];

            Log::info() << latitudes[i] << " " << longitudes[i] << std::endl;
        }
        s.close();
    }
    else {
        NOTIMP;
    }
}


UnstructuredGrid::UnstructuredGrid(const std::vector<double>& latitudes, const std::vector<double>& longitudes,
                                   const BoundingBox& bbox) :
    Grid(bbox), latitudes_(latitudes), longitudes_(longitudes) {
    ASSERT(latitudes_.size() == longitudes_.size());
    check_duplicate_points("UnstructuredGrid from arguments", latitudes_, longitudes_);
}


UnstructuredGrid::~UnstructuredGrid() = default;


void UnstructuredGrid::print(std::ostream& out) const {
    out << "UnstructuredGrid[points=" << numberOfPoints() << "]";
}


bool UnstructuredGrid::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const UnstructuredGrid*>(&other);
    return (o != nullptr) && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_);
}


Domain UnstructuredGrid::domain() const {
    // FIXME Should be global?
    return {bbox().north(), bbox().west(), bbox().south(), bbox().east()};
}


size_t UnstructuredGrid::numberOfPoints() const {
    ASSERT(latitudes_.size() == longitudes_.size());
    return latitudes_.size();
}


const Grid* UnstructuredGrid::croppedGrid(const BoundingBox& bbox) const {
    std::vector<double> lat;
    std::vector<double> lon;

    size_t i = 0;
    size_t j = 0;

    for (const std::unique_ptr<Iterator> iter(iterator()); iter->next(); ++i) {
        if (bbox.contains(iter->pointUnrotated())) {
            auto ip = iter->index();
            lat.emplace_back(latitudes_.at(ip));
            lon.emplace_back(longitudes_.at(ip));
            ++j;
        }
    }

    if (j < i) {
        Log::debug() << "UnstructuredGrid::croppedGrid: cropped " << Log::Pretty(i) << " to "
                     << Log::Pretty(j, {"point"}) << std::endl;
        ASSERT(j);
        return new UnstructuredGrid(lat, lon, bbox);
    }

    Log::debug() << "UnstructuredGrid::croppedGrid: no cropping" << std::endl;
    return this;
}


Iterator* UnstructuredGrid::iterator() const {
    return new iterator::UnstructuredIterator(latitudes_, longitudes_);
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


template <>
Grid* GridBuilder<other::UnstructuredGrid>::make(const Configuration& param) {
#if mir_HAVE_ATLAS
    // specially-named unstructured grids
    std::string grid;
    if (param.get("grid", grid)) {
        if (!key::grid::ORCAPattern::match(grid, param).empty()) {
            return new other::ORCA(param);
        }
    }
#endif

    return new other::UnstructuredGrid(param);
}


}  // namespace eckit::geo::grid
