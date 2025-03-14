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


#include "eckit/geo/Shapefile.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/Unzip.h"
#include "eckit/geo/polygon/LonLatPolygon.h"
#include "eckit/geo/spec/Custom.h"

#include "shapefil.h"


namespace eckit::geo {


struct DBF : std::unique_ptr<DBFInfo, decltype(&DBFClose)> {
    explicit DBF(const PathName& path) : unique_ptr{DBFOpen(path.localPath(), "rb"), DBFClose} {
        if (!operator bool()) {
            throw CantOpenFile(path + " (as .dbf)", Here());
        }
    }

    std::map<std::string, int> string_fields() const {
        std::map<std::string, int> m;

        char name[12];
        for (int i = 0, n = DBFGetFieldCount(get()); i < n; ++i) {
            if (DBFGetNativeFieldType(get(), i) == 'C') {
                DBFGetFieldInfo(get(), i, name, nullptr, nullptr);
                m[name] = i;
            }
        }

        return m;
    }

    std::string read_string_attribute(int record, int field) const {
        return DBFReadStringAttribute(get(), record, field);
    }
};


struct SHP : std::unique_ptr<SHPInfo, decltype(&SHPClose)> {
    struct Object : std::unique_ptr<SHPObject, decltype(&SHPDestroyObject)> {
        explicit Object(SHPObject* ptr) : unique_ptr{ptr, SHPDestroyObject} { ASSERT(operator bool()); }
    };

    explicit SHP(const PathName& path) : unique_ptr{SHPOpen(path.localPath(), "rb"), SHPClose} {
        if (!operator bool()) {
            throw CantOpenFile(path + " (as .shp)", Here());
        }

        int type = 0;
        SHPGetInfo(get(), &nEntities_, &type, nullptr, nullptr);

        if (type != SHPT_ARC && type != SHPT_POLYGON) {
            throw ReadError("Shapefile: unsupported shape type", Here());
        }
    }

    int n_entities() const { return nEntities_; }

    Area* make_area(int entity) const {
        Object obj(SHPReadObject(get(), entity));
        ASSERT(obj);
        ASSERT(obj->nSHPType == SHPT_ARC || obj->nSHPType == SHPT_POLYGON);

        area::Polygon::container_type parts;

        using points_type = polygon::LonLatPolygon::container_type;

        for (int p = 0; p < obj->nParts; ++p) {
            // only process closed loops
            if (int start = obj->panPartStart[p],
                end       = (p == obj->nParts - 1) ? obj->nVertices : obj->panPartStart[p + 1];
                start < end) {
                if (points_type::value_type first(obj->padfX[start], obj->padfY[start]),
                    last(obj->padfX[end - 1], obj->padfY[end - 1]);
                    points_equal(first, last)) {
                    points_type pts;

                    pts.reserve(end - start);
                    for (int j = start; j < end; ++j) {
                        pts.emplace_back(obj->padfX[j], obj->padfY[j]);
                    }

                    parts.emplace_back(pts);
                }
            }
        }

        return new area::Polygon(parts);
    }

private:
    int nEntities_ = 0;
};


Shapefile::Shapefile(const Spec& spec) : Shapefile(spec.get_string("file")) {}


Shapefile::Shapefile(const PathName& file, const std::string& field) :
    shp_(file.extension() == ".zip" ? [](const PathName& zip) -> PathName {
        static cache::Unzip unzip(LibEcKitGeo::cacheDir() + "/shapefile");

        auto dir = unzip.to_cached_path(zip);

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        dir.children(files, dirs);

        files.erase(
            std::remove_if(files.begin(), files.end(), [](const auto& path) { return path.extension() != ".shp"; }),
            files.end());

        if (files.size() != 1) {
            throw ReadError("Shapefile: found none/more than one .shp file(s) in '" + zip + "', expecting one", Here());
        }

        return files.front();
    }(file) : file), dbf_(shp_.asString().substr(0, shp_.asString().length() - 4) + ".dbf"), field_(field) {
    SHP shp(shp_);
    DBF dbf(dbf_);

    const auto nameFieldIndex = field_.empty() ? 0 : dbf.string_fields().at(field_);

    for (int e = 0; e < shp.n_entities(); ++e) {
        /*auto poly =*/shp.make_area(e);

        // std::cout << "Arc/Polygon '" << dbf.read_string_attribute(e, nameFieldIndex) << "'" << std::endl;
        // std::cout << poly << std::endl;
    }
}


Shapefile* Shapefile::make_from_url(const std::string& url) {
    static cache::Download download(LibEcKitGeo::cacheDir() + "/shapefile");

    std::cout << "Shapefile::make_from_url: " << url << std::endl;

    auto path = download.to_cached_path(url, cache::Download::url_file_basename(url, false),
                                        cache::Download::url_file_extension(url));

    if (path.extension() == ".zip") {
        return Shapefile::make_from_zip(path);
    }

    ASSERT(path.extension() == ".shp");
    return new Shapefile(path);
}


void Shapefile::fill_spec(spec::Custom& custom) const {
    custom.set("type", "shapefile");
    custom.set("file", shp_);
}


Shapefile* Shapefile::make_from_zip(const PathName& zip) {
    std::vector<std::string> whats;
    for (const auto& what : cache::Unzip::list(zip, true)) {
        if (PathName(what).extension() == ".shp") {
            whats.push_back(what);
        }
    }

    if (whats.size() != 1) {
        throw ReadError("Shapefile: found none/more than one .shp file(s) in '" + zip + "', expecting one", Here());
    }

    auto dir = zip + ".dir";
    if (!dir.exists()) {
        cache::Unzip unzip(LibEcKitGeo::cacheDir() + "/shapefile");
        unzip.to_cached_path(zip);
    }

    auto path_shp = dir / whats.front();
    ASSERT(path_shp.exists());
    return new Shapefile(path_shp);
}


std::ostream& Shapefile::list(std::ostream& out) const {
    NOTIMP;
}


Area* Shapefile::make_area_from_string(const std::string&) const {}


Area* Shapefile::make_area_from_int(int) const {
    return SHP(shp_).make_area(0);
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
