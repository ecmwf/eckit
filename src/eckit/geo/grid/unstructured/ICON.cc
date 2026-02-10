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


#include "eckit/geo/grid/unstructured/ICON.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "eckit/codec/codec.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Record.h"
#include "eckit/geo/container/PointsContainer.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::grid::unstructured {


static const ICON::ICONRecord& icon_record(const spec::Spec& spec) {
    static cache::Record<ICON::ICONRecord> cache(LibEcKitGeo::cacheDir() + "/grid/icon");
    return cache.get(spec);
}


ICON::ICON(const ICONRecord& record, const uid_type& uid, const std::string& arrangement, const std::string& name) :
    Unstructured(new container::PointsLonLatReference{record.longitudes_, record.latitudes_}),
    name_(name),
    arrangement_(arrangement_from_string(arrangement)),
    record_(record) {}


ICON::ICON(const Spec& spec) :
    ICON(icon_record(spec), spec.get_string("icon_uid"), spec.get_string("icon_arrangement"), spec.get_string("name")) {
    ASSERT(container());
    reset_uid(spec.get_string("icon_uid"));
}


ICON::ICON(uid_type uid) : ICON(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


ICON::ICON(const std::string& name, Arrangement a) :
    ICON(*std::unique_ptr<Spec>(
        GridFactory::make_spec(spec::Custom({{"grid", name + '_' + arrangement_to_string(a)}})))) {}


std::string ICON::arrangement() const {
    return arrangement_to_string(arrangement_);
}


ICON::ICONRecord::bytes_t ICON::ICONRecord::footprint() const {
    return sizeof(longitudes_.front()) * longitudes_.size() + sizeof(latitudes_.front()) * latitudes_.size();
}


size_t ICON::ICONRecord::n() const {
    return latitudes_.size();
}


void ICON::ICONRecord::read(const PathName& p) {
    codec::RecordReader reader(p);

    int32_t version = 0;
    reader.read("version", version).wait();

    if (version == 0) {
        reader.read("latitude", latitudes_);
        reader.read("longitude", longitudes_);
        reader.wait();

        ASSERT(latitudes_.size() == longitudes_.size());
        return;
    }

    throw SeriousBug("ICON: unsupported version", Here());
}


void ICON::ICONRecord::check(const Spec& spec) const {
    auto _n = static_cast<size_t>(n());
    ASSERT(_n > 0);

    if (std::vector<size_t> shape; spec.get("shape", shape)) {
        ASSERT(shape.size() == 1 && shape.front() == _n);
    }

    ASSERT(_n == longitudes_.size());
    ASSERT(_n == latitudes_.size());
}


Grid::Spec* ICON::spec(const std::string& name) {
    return GridSpecByUID::instance().get(name).spec();
}


Arrangement ICON::arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::ICON_C
           : str == "V" ? Arrangement::ICON_V
           : str == "E" ? Arrangement::ICON_E
                        : throw SeriousBug("ICON: unsupported arrangement '" + str + "'");
}


std::string ICON::arrangement_to_string(Arrangement a) {
    return a == Arrangement::ICON_C   ? "C"
           : a == Arrangement::ICON_V ? "V"
           : a == Arrangement::ICON_E
               ? "E"
               : throw SeriousBug("ICON: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


void ICON::fill_spec(spec::Custom& custom) const {
    custom.set("grid", name_);  // only T-arrangement is currently catalogued

    if (auto _uid = uid(); !GridSpecByUID::instance().exists(_uid)) {
        custom.set("uid", _uid);
    }
}


const std::string& ICON::type() const {
    static const std::string type{"ICON"};
    return type;
}


Grid::uid_type ICON::calculate_uid() const {
    NOTIMP;
}


Grid::BoundingBox* ICON::calculate_bbox() const {
    if (const std::string BOUNDING_BOX = "bounding_box"; catalog().has(BOUNDING_BOX)) {
        const auto bbox = catalog().get_double_vector(BOUNDING_BOX);
        ASSERT(bbox.size() == 4);

        return new BoundingBox{bbox[0], bbox[1], bbox[2], bbox[3]};
    }

    return Unstructured::calculate_bbox();
}


Point ICON::first_point() const {
    ASSERT(!empty());
    return PointLonLat{record_.longitudes_.front(), record_.latitudes_.front()};
}


Point ICON::last_point() const {
    ASSERT(!empty());
    return PointLonLat{record_.longitudes_.back(), record_.latitudes_.back()};
}


static const GridRegisterType<ICON> GRIDTYPE("ICON");


}  // namespace eckit::geo::grid::unstructured
