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


#include "eckit/geometry/grid/UnstructuredGrid.h"

#include <fstream>
#include <memory>
#include <set>
#include <sstream>
#include <utility>

#include "eckit/exception/Exceptions.h"

#if 0
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geometry/Domain.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Projection.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/IfstreamStream.h"
#endif


namespace eckit::geometry::grid {


static const area::BoundingBox __global;


UnstructuredGrid::UnstructuredGrid(std::vector<Point>&& points) :
    Grid(__global), points_(points) {
}


Grid::iterator UnstructuredGrid::cbegin() const {
    NOTIMP;
}


Grid::iterator UnstructuredGrid::cend() const {
    NOTIMP;
}


UnstructuredGrid::~UnstructuredGrid() = default;


size_t UnstructuredGrid::size() const {
    return points_.size();
}


// static const GridRegisterType<UnstructuredGrid> unstructured_grid("unstructured_grid");


}  // namespace eckit::geometry::grid
