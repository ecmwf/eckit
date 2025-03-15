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


#include "eckit/geo/area/library/Shapefile.h"

#include <algorithm>
#include <memory>
#include <ostream>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/Unzip.h"
#include "eckit/geo/polygon/LonLatPolygon.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/log/JSON.h"


namespace eckit::geo::area::library {


namespace {


PathName path_shp(const PathName& file) {
    if (file.extension() == ".zip") {
        // check for exactly one .shp file in the zip
        if (auto list = cache::Unzip::list(file, true);
            std::count_if(list.begin(), list.end(),
                          [](const auto& what) { return PathName(what).extension() == ".shp"; })
            != 1) {
            throw ReadError("Shapefile: found none/more than one .shp file(s) in '" + file + "', expecting one",
                            Here());
        }

        static cache::Unzip unzip(LibEcKitGeo::cacheDir() + "/shapefile");

        auto dir = unzip.to_cached_path(file);

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        dir.children(files, dirs);

        files.erase(
            std::remove_if(files.begin(), files.end(), [](const auto& file) { return file.extension() != ".shp"; }),
            files.end());

        if (files.size() != 1) {
            throw ReadError("Shapefile: found none/more than one .shp file(s) in '" + file + "', expecting one",
                            Here());
        }

        return files.front();
    }

    return file;
}


PathName path_dbf(const PathName& file, const PathName& shp) {
    if (file.asString().empty()) {
        auto str = shp.asString();
        return str.substr(0, str.length() - 4) + ".dbf";
    }

    return file;
}


}  // namespace


Shapefile::Shapefile(const Spec& spec) :
    Shapefile(path_shp(spec.get_string(spec.has("shp") ? "shp" : "file")),
              spec.has("dbf") ? spec.get_string("dbf") : "",
              spec.has("name_field") ? spec.get_string("name_field") : "") {}


Shapefile::Shapefile(const PathName& file) : Shapefile(file, "") {}


Shapefile::Shapefile(const PathName& shp, const PathName& dbf, const std::string& name) :
    shpPath_(path_shp(shp)), dbfPath_(path_dbf(dbf, shpPath_)), nEntities_(0) {
    if ((shp_ = SHPOpen(shpPath_.localPath(), "rb")) == nullptr) {
        throw CantOpenFile(shpPath_ + " (as .shp)", Here());
    }

    int type = 0;
    SHPGetInfo(shp_, &nEntities_, &type, nullptr, nullptr);

    if (type != SHPT_ARC && type != SHPT_POLYGON) {
        throw ReadError("Shapefile: unsupported shape type", Here());
    }

    if (!name.empty()) {
        DBFInfo* dbf = DBFOpen(dbfPath_.localPath(), "rb");
        if (dbf == nullptr) {
            throw CantOpenFile(shpPath_ + " (as .dbf)", Here());
        }

        // find named field index
        std::map<std::string, int> to_index;

        char fieldName[12];
        for (int i = 0, n = DBFGetFieldCount(dbf); i < n; ++i) {
            if (DBFGetNativeFieldType(dbf, i) == 'C') {
                DBFGetFieldInfo(dbf, i, fieldName, nullptr, nullptr);
                if (name == fieldName) {
                    name_ = name;

                    // map entities names (they have to be unique)
                    for (int e = 0; e < nEntities_; ++e) {
                        ASSERT(to_entity.emplace(DBFReadStringAttribute(dbf, e, i), e).second);
                    }

                    break;
                }
            }
        }

        DBFClose(dbf);

        if (name_.empty()) {
            throw ReadError("Shapefile: field '" + name + "' not found in '" + dbfPath_ + "'", Here());
        }
    }
}


Shapefile::~Shapefile() {
    SHPClose(shp_);
}


Shapefile* Shapefile::make_from_url(const std::string& url) {
    static cache::Download download(LibEcKitGeo::cacheDir() + "/shapefile");

    return new Shapefile(download.to_cached_path(url, cache::Download::url_file_basename(url, false),
                                                 cache::Download::url_file_extension(url)));
}


void Shapefile::fill_spec(spec::Custom& custom) const {
    custom.set("type", "shapefile");
    custom.set("shp", shpPath_);
    custom.set("shp", dbfPath_);
    if (!name_.empty()) {
        custom.set("field", name_);
    }
}


std::ostream& Shapefile::list(std::ostream& out) const {
    JSON j(out);
    j.startObject();

    j << "type" << "shapefile";
    j << "shp" << shpPath_;
    j << "dbf" << dbfPath_;
    j << "name" << name_;
    j << "n_entities" << size();
    j << "entities";

    j.startList();
    for (const auto& [key, val_] : to_entity) {
        j << key;
    }
    j.endList();

    j.endObject();
    return out;
}


Area* Shapefile::make_area_from_name(const std::string& name) const {
    ASSERT(!name_.empty());
    ASSERT(!name.empty());

    return make_area(to_entity.at(name));
}


Area* Shapefile::make_area(size_t entity) const {
    struct Object : std::unique_ptr<SHPObject, decltype(&SHPDestroyObject)> {
        explicit Object(SHPObject* ptr) : unique_ptr{ptr, SHPDestroyObject} { ASSERT(operator bool()); }
    };

    Object obj(SHPReadObject(shp_, static_cast<int>(entity)));
    ASSERT(obj);
    ASSERT(obj->nSHPType == SHPT_ARC || obj->nSHPType == SHPT_POLYGON);

    area::Polygon::container_type parts;

    for (int p = 0; p < obj->nParts; ++p) {
        auto start = obj->panPartStart[p];
        auto end   = (p == obj->nParts - 1) ? obj->nVertices : obj->panPartStart[p + 1];

        if (start < end) {
            using points_type = polygon::LonLatPolygon::container_type;

            // only process closed loops
            points_type::value_type first(obj->padfX[start], obj->padfY[start]);
            points_type::value_type last(obj->padfX[end - 1], obj->padfY[end - 1]);

            if (points_equal(first, last)) {
                points_type pts;
                pts.reserve(end - start);

                for (auto j = start; j < end; ++j) {
                    pts.emplace_back(obj->padfX[j], obj->padfY[j]);
                }

                parts.emplace_back(pts);
            }
        }
    }

    return new area::Polygon(parts);
}


#if 0
class AreaLibrary {
public:
    void add_library(const std::string& lib, const Value& value) {
        std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(value));

        emplace(lib, ShapefileURL{spec->get_string("url_prefix", "") + spec->get_string("url")});
    }
};
#endif


}  // namespace eckit::geo
