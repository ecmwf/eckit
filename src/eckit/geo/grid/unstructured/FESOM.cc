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


#include "eckit/geo/grid/unstructured/FESOM.h"

#include "eckit/codec/codec.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Download.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_coordinate(MD5&, const std::vector<double>&, bool _byteswap);
}


namespace eckit::geo::grid::unstructured {


static std::string fesom_path(const Download::url_type& url) {
    static Download download(LibEcKitGeo::cacheDir() + "/grid/fesom", "fesom-", ".codec");
    return download.to_cached_path(url);
}


FESOM::FESOM(const Spec& spec) :
    Unstructured(spec),
    uid_(spec.get_string("fesom_uid")),
    arrangement_(arrangement_from_string(spec.get_string("fesom_arrangement"))),
    path_(fesom_path(spec.get_string("url_prefix", "") + spec.get_string("url"))) {
    codec::RecordReader reader(path_);

    uint64_t version = -1;
    reader.read("version", version).wait();

    if (version == 0) {
        uint64_t n = 0;
        reader.read("n", n);

        std::vector<double> longitudes;
        std::vector<double> latitudes;
        reader.read("latitude", latitudes);
        reader.read("longitude", longitudes);
        reader.wait();

        ASSERT(n == latitudes.size());
        ASSERT(n == longitudes.size());

        return;
    }

    throw SeriousBug("FESOM: unsupported version", Here());
}


FESOM::FESOM(uid_t uid) : FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


Grid::uid_t FESOM::calculate_uid() const {
    MD5 hash;

    util::hash_coordinate(hash, latitudes_, !eckit_LITTLE_ENDIAN);
    util::hash_coordinate(hash, longitudes_, !eckit_LITTLE_ENDIAN);

    auto d = hash.digest();
    ASSERT(d.length() == 32);

    return {d};
}


void FESOM::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", "FESOM");
    custom.set("uid", uid());
}


static const GridRegisterType<FESOM> GRIDTYPE("FESOM");
// static const GridRegisterName<FESOM> GRIDNAME(GridRegisterName<FESOM>::uid_pattern);


}  // namespace eckit::geo::grid::unstructured
