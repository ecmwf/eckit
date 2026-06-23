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

#include <memory>

#include "eckit/codec/codec.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {
void hash_vector_double(MD5&, const std::vector<double>&);
void hash_vector_size_t(MD5&, const std::vector<size_t>&);
}  // namespace eckit::geo::util


namespace eckit::geo::grid::unstructured {


static Arrangement arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::FESOM_C
           : str == "N" ? Arrangement::FESOM_N
                        : throw SeriousBug("FESOM: unsupported arrangement '" + str + "'");
}


static std::string arrangement_to_string(Arrangement a) {
    return a == Arrangement::FESOM_C ? "C"
           : a == Arrangement::FESOM_N
               ? "N"
               : throw SeriousBug("ORCA: unsupported arrangement '" + std::to_string(a) + "'", Here());
}


FESOM::FESOM(const Spec& spec) :
    Unstructured(spec.get_string("uid"), spec.get_string("name"),
                 arrangement_to_string(arrangement_from_string(spec.get_string("arrangement")))) {}


FESOM::FESOM(const uid_type& uid) :
    FESOM(*std::unique_ptr<Spec>(GridFactory::make_spec(spec::Custom({{"uid", uid}})))) {}


FESOM::FESOM(const std::string& name, Arrangement a) :
    FESOM(*[](const std::string& grid) {
        return std::unique_ptr<const spec::Spec>(GridSpecByName::instance().match(grid).spec(grid));
    }(name + "_" + arrangement_to_string(a))) {}


const std::string& FESOM::type() const {
    static const std::string type{"FESOM"};
    return type;
}


std::vector<size_t> FESOM::shape() const {
    return catalog().get_unsigned_vector("shape");
}


Grid::uid_type FESOM::calculate_uid() const {
    if (arrangement_from_string(arrangement()) == Arrangement::FESOM_N) {
        return Unstructured::calculate_uid();
    }

    if (arrangement_from_string(arrangement()) == Arrangement::FESOM_C) {
        // download internally controls concurrent reads/writes
        static cache::Download download(PathName{LibEcKitGeo::cacheDir()} / "fesom" /
                                        std::to_string(cache::Download::version()));

        // bootstrap uid
        std::unique_ptr<Spec> spec(GridSpecByUID::instance().get(uid()).spec());

        // get coordinates from arrangement: N
        auto [lat, lon] = FESOM(spec->get_string("name"), Arrangement::FESOM_N).to_latlons();

        // get element indices (0-based) from arrangement: C (this one)
        auto url  = LibEcKitGeo::url(spec->get_string("url_connectivity"));
        auto path = download.to_cached_path(url, spec->get_string("name", ""), ".ek");
        ASSERT_MSG(path.exists(), "FESOM: file '" + path + "' not found");

        codec::RecordReader reader(path);

        uint64_t version = 0;
        reader.read("version", version).wait();
        ASSERT(version == 0);

        std::vector<size_t> elem2d;
        reader.read("elem2d", elem2d).wait();

        MD5 hash;
        util::hash_vector_double(hash, lat);
        util::hash_vector_double(hash, lon);
        util::hash_vector_size_t(hash, elem2d);

        auto d = hash.digest();
        ASSERT(is_uid(d));
        return {d};
    }

    NOTIMP;
}


Grid::Spec* FESOM::spec(const std::string& str) {
    return is_uid(str) ? GridSpecByUID::instance().get(str).spec() : GridSpecByName::instance().get(str).spec(str);
}


static const GridRegisterType<FESOM> GRIDTYPE("FESOM");


}  // namespace eckit::geo::grid::unstructured
