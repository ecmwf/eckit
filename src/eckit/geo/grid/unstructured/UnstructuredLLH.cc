// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/unstructured/UnstructuredLLH.h"

#include <map>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Custom.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
}


namespace eckit::geo::grid::unstructured {


namespace {


const std::string TYPE{"unstructured_llh"};


struct lock_tag;  // of this file's mutex
using lock_type = util::lock_type<lock_tag>;


// not owning, so the coordinates go with the last grid using them
std::map<Grid::uid_type, std::weak_ptr<const UnstructuredLLH::Coordinates>>& registry() {
    static std::map<Grid::uid_type, std::weak_ptr<const UnstructuredLLH::Coordinates>> registry;
    return registry;
}


}  // namespace


UnstructuredLLH::UnstructuredLLH(const Spec& spec) :
    UnstructuredLLH(spec.has("uid") ? find(spec.get_string("uid"))
                                    : regist({spec.get_double_vector("longitudes"), spec.get_double_vector("latitudes"),
                                              spec.get_double_vector("heights")}),
                    projection_from_spec(spec)) {}


UnstructuredLLH::UnstructuredLLH(std::vector<double> longitudes, std::vector<double> latitudes,
                                 std::vector<double> heights) :
    UnstructuredLLH(regist({std::move(longitudes), std::move(latitudes), std::move(heights)}), nullptr) {}


UnstructuredLLH::UnstructuredLLH(Registered&& registered, std::unique_ptr<const Projection> p) :
    Grid(nullptr, std::move(p)), coordinates_(std::move(registered.second)) {
    ASSERT(coordinates_);
    reset_uid(registered.first);
}


Point UnstructuredLLH::point(size_t index) const {
    const auto& [lon, lat, height] = *coordinates_;
    return PointLonLatH{lon.at(index), lat[index], height[index]};
}


std::pair<std::vector<double>, std::vector<double>> UnstructuredLLH::to_latlons() const {
    return {(*coordinates_)[1], (*coordinates_)[0]};
}


const std::string& UnstructuredLLH::type() const {
    return TYPE;
}


Grid::BoundingBox* UnstructuredLLH::calculate_bbox() const {
    return new BoundingBox;
}


Grid::uid_type UnstructuredLLH::calculate_uid() const {
    return uid_of(*coordinates_);
}


void UnstructuredLLH::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", TYPE);
    custom.set("uid", uid());
}


Grid::uid_type UnstructuredLLH::uid_of(const Coordinates& coordinates) {
    MD5 hash;
    hash.add(TYPE);
    for (const auto& v : coordinates) {
        util::hash_vector_double(hash, v);
    }

    auto d = hash.digest();
    ASSERT(is_uid(d));
    return d;
}


UnstructuredLLH::Registered UnstructuredLLH::regist(Coordinates&& coordinates) {
    if (coordinates[0].size() != coordinates[1].size() || coordinates[0].size() != coordinates[2].size()) {
        throw exception::GridError("UnstructuredLLH: coordinates of different sizes", Here());
    }

    const auto id = uid_of(coordinates);

    lock_type lock;

    // the same points as in use, sharing them
    auto& registered = registry()[id];
    auto shared      = registered.lock();
    if (!shared) {
        shared     = std::make_shared<const Coordinates>(std::move(coordinates));
        registered = shared;
        GridSpecByUID::replace(id, spec::Custom{{"type", TYPE}, {"uid", id}});
    }

    return {id, shared};
}


UnstructuredLLH::Registered UnstructuredLLH::find(const uid_type& _uid) {
    const auto id = StringTools::lower(_uid);

    lock_type lock;

    if (auto it = registry().find(id); it != registry().end()) {
        if (auto shared = it->second.lock(); shared) {
            return {id, shared};
        }
    }

    throw exception::GridUnknownError("UnstructuredLLH: coordinates of uid '" + id + "' not in use in this process",
                                      Here());
}


static const GridRegisterType<UnstructuredLLH> GRIDTYPE(TYPE);


}  // namespace eckit::geo::grid::unstructured
