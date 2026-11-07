// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/unstructured/FESOM.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

#include "eckit/cache/Download.h"
#include "eckit/codec/codec.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
void hash_vector_size_t(MD5&, const std::vector<size_t>&);
}  // namespace eckit::geo::util


namespace eckit::geo::grid::unstructured {


namespace {


// Mesh of triangles: nodes (lat, lon) and cells (0-based node indices, 3 per cell)
struct Mesh {
    explicit Mesh(const Grid::Spec& spec) {
        // download internally controls concurrent reads/writes
        static const eckit::cache::Download download(LibEcKitGeo::cacheRoots() / "download");

        auto path = download.toCachedPath(LibEcKitGeo::url(spec.get_string("url")), "", ".ek");
        ASSERT_MSG(path.exists(), "FESOM: file '" + path + "' not found");

        codec::RecordReader reader(path);

        int32_t version = 0;
        reader.read("version", version).wait();
        ASSERT(version == 0);

        std::vector<int64_t> cell2node;
        reader.read("latitude", lat);
        reader.read("longitude", lon);
        reader.read("connectivity_cell2node", cell2node);
        reader.wait();

        ASSERT(lat.size() == lon.size());
        ASSERT(cell2node.size() % 3 == 0);

        cells.assign(cell2node.begin(), cell2node.end());
        ASSERT(std::all_of(cells.begin(), cells.end(), [n = lat.size()](size_t k) { return k < n; }));
    }

    /// Cell centres (as FESOM's elem_center): mean of node latitudes, and of node longitudes, unwrapped eastwards of
    /// the westmost node
    cache::LatitudeLongitude centres() const {
        auto unwrap = [](double x, double lon_minimum) {
            return x - PointLonLat::FLAT_ANGLE >= lon_minimum ? x - PointLonLat::FULL_ANGLE : x;
        };

        const auto n = cells.size() / 3;
        std::vector<double> clat(n);
        std::vector<double> clon(n);

        for (size_t c = 0; c < n; ++c) {
            const auto* k = &cells[3 * c];
            const auto w  = std::min({lon[k[0]], lon[k[1]], lon[k[2]]});  // west-most

            clat[c] = (lat[k[0]] + lat[k[1]] + lat[k[2]]) / 3.;
            clon[c] = (unwrap(lon[k[0]], w) + unwrap(lon[k[1]], w) + unwrap(lon[k[2]], w)) / 3.;
        }

        return {std::move(clat), std::move(clon)};
    }

    std::vector<double> lat;
    std::vector<double> lon;
    std::vector<size_t> cells;
};


cache::LatitudeLongitude load(const Grid::Spec& spec) {
    Mesh mesh(spec);
    return arrangement_from_string("FESOM", spec.get_string("arrangement")) == Arrangement::FESOM_C
               ? mesh.centres()
               : cache::LatitudeLongitude{std::move(mesh.lat), std::move(mesh.lon)};
}


}  // namespace


FESOM::FESOM(const Spec& spec) :
    Unstructured(spec.get_string("uid"), spec.get_string("name"),
                 arrangement_to_string(arrangement_from_string("FESOM", spec.get_string("arrangement"))),
                 bounding_box_from_spec(spec), ProjectionFactory::build(spec)) {}


FESOM::FESOM(const uid_type& uid) :
    FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


FESOM::FESOM(const std::string& name, Arrangement a) :
    FESOM(*[](const std::string& grid) {
        return std::unique_ptr<const spec::Spec>(GridSpecByName::instance().match_spec(grid, grid));
    }(name + "_" + arrangement_to_string(a))) {}


const std::string& FESOM::type() const {
    static const std::string type{"FESOM"};
    return type;
}


std::vector<size_t> FESOM::shape() const {
    return catalog().get_unsigned_vector("shape");
}


Grid::uid_type FESOM::calculate_uid() const {
    if (arrangement_from_string("FESOM", arrangement()) == Arrangement::FESOM_N) {
        return Unstructured::calculate_uid();
    }

    // cells, by the nodes and the cells (node indices)
    const Mesh mesh(catalog());

    MD5 hash;
    util::hash_vector_double(hash, mesh.lat);
    util::hash_vector_double(hash, mesh.lon);
    util::hash_vector_size_t(hash, mesh.cells);

    auto d = hash.digest();
    ASSERT(is_uid(d));
    return {d};
}


Grid::Spec* FESOM::spec(const std::string& str) {
    return is_uid(str) ? GridSpecByUID::instance().get_spec(str) : GridSpecByName::instance().get_spec(str, str);
}


static const GridRegisterType<FESOM> GRIDTYPE("FESOM");
static const struct LoadRegister {
    LoadRegister() { cache::LatitudeLongitude::register_load("FESOM", load); }
} LOAD;


}  // namespace eckit::geo::grid::unstructured
