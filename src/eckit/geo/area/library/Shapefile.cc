// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/area/library/Shapefile.h"

#include <algorithm>
#include <ostream>
#include <utility>
#include <vector>

#include "shapefil.h"

#include "eckit/cache/Download.h"
#include "eckit/cache/Unzip.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/geo/polygon/Polygon.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Layered.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::area::library {


// keeps <shapefil.h> out of the installed header
struct Shapefile::Implementation {
    SHPInfo* shp = nullptr;
};


namespace {


PathName path_shp(const PathName& file) {
    if (file.extension() == ".zip") {
        // check for exactly one .shp file in the zip
        if (auto list = eckit::cache::Unzip::list(file, true);
            std::count_if(list.begin(), list.end(),
                          [](const auto& what) { return PathName(what).extension() == ".shp"; }) != 1) {
            throw ReadError("Shapefile: found none/more than one .shp file(s) in '" + file + "', expecting one",
                            Here());
        }

        static eckit::cache::Unzip unzip(LibEcKitGeo::cacheRoots() / "shapefile");

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


PathName download(const std::string& url) {
    static const eckit::cache::Download download(LibEcKitGeo::cacheRoots() / "download");

    const auto u = LibEcKitGeo::url(url);
    return download.to_cached_path(u, eckit::cache::Download::url_file_basename(u, false),
                                   eckit::cache::Download::url_file_extension(u));
}


/// Polygons of a shapefile entity, specified by the shapefile (multiple rings have no polygon spec)
class Entity final : public Polygon {
public:

    Entity(container_type&& rings, const spec::Custom& spec) : Polygon(std::move(rings)), spec_(spec.container()) {}
    Entity(const Entity& other) : Polygon(other), spec_(other.spec_.container()) {}
    explicit Entity(const Area::Spec& spec) : Entity(*std::unique_ptr<Entity>(make_from_spec(spec))) {}

    const std::string& type() const override {
        static const std::string type{"shapefile"};
        return type;
    }

    [[nodiscard]] static Entity* make_from_spec(const Area::Spec& spec) {
        if (!spec.has("name") && !spec.has("index")) {
            throw exception::AreaError("Shapefile: 'name' (with 'name_field') or 'index' expected", Here());
        }

        // a registered library (see shapefile.yaml) provides the shapefile, overridable by the spec
        std::unique_ptr<spec::Layered> layered;
        if (spec.has("library")) {
            layered = std::make_unique<spec::Layered>(spec);
            layered->push_back(AreaFactory::library(spec.get_string("library")));
        }

        const Shapefile shapefile(layered ? *layered : spec);
        auto* area = spec.has("name") ? shapefile.make_area_from_name(spec.get_string("name"))
                                      : shapefile.make_area(spec.get_unsigned("index"));

        auto* entity = dynamic_cast<Entity*>(area);
        ASSERT(entity != nullptr);
        return entity;
    }

private:

    void fill_spec(spec::Custom& custom) const override { custom.set(spec_); }

    const spec::Custom spec_;
};


const AreaRegisterType<Entity> AREATYPE("shapefile");


}  // namespace


Shapefile::Shapefile(const spec::Spec& spec) :
    Shapefile(spec.has("url") ? download(spec.get_string("url"))
                              : PathName{spec.get_string(spec.has("shp") ? "shp" : "file")},
              spec.get_string("dbf", ""), spec.get_string("name_field", "")) {
    if (spec.has("url")) {
        url_ = spec.get_string("url");
    }

    if (spec.has("library")) {
        library_ = spec.get_string("library");
    }
}


Shapefile::Shapefile(const PathName& file) : Shapefile(file, "") {}


Shapefile::Shapefile(const PathName& shp, const PathName& dbf, const std::string& name) :
    file_(shp.realName()),
    dbf_(dbf),
    shpPath_(path_shp(shp)),
    dbfPath_(path_dbf(dbf, shpPath_)),
    impl_(std::make_unique<Implementation>()),
    nEntities_(0) {
    Log::debug<LibEcKitGeo>() << "eckit::geo::area::library::Shapefile(shp='" << shpPath_.realName() << "',dbf='"
                              << dbfPath_.realName() << "',name='" << name << "')" << std::endl;

    if ((impl_->shp = SHPOpen(shpPath_.localPath(), "rb")) == nullptr) {
        throw CantOpenFile(shpPath_ + " (as .shp)", Here());
    }

    int type = 0;
    SHPGetInfo(impl_->shp, &nEntities_, &type, nullptr, nullptr);

    if (type != SHPT_ARC && type != SHPT_POLYGON) {
        throw ReadError("Shapefile: unsupported shape type", Here());
    }

    if (!name.empty()) {
        DBFInfo* dbf = DBFOpen(dbfPath_.localPath(), "rb");
        if (dbf == nullptr) {
            throw CantOpenFile(shpPath_ + " (as .dbf)", Here());
        }

        std::map<std::string, int> to_index;

        char fieldName[12];
        for (int i = 0, n = DBFGetFieldCount(dbf); i < n; ++i) {
            if (DBFGetNativeFieldType(dbf, i) == 'C') {
                DBFGetFieldInfo(dbf, i, fieldName, nullptr, nullptr);
                if (name == fieldName) {
                    name_ = name;

                    // map entities names (ambiguous if not unique)
                    for (int e = 0; e < nEntities_; ++e) {
                        if (auto [it, unique] = to_entity.emplace(DBFReadStringAttribute(dbf, e, i), e); !unique) {
                            it->second = -1;
                        }
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
    SHPClose(impl_->shp);
}


Shapefile* Shapefile::make_from_url(const std::string& url) {
    return new Shapefile(spec::Custom{{"url", url}});
}


void Shapefile::fill_spec(spec::Custom& custom) const {
    custom.set("type", "shapefile");
    custom.set("shp", shpPath_);
    custom.set("dbf", dbfPath_);
    if (!name_.empty()) {
        custom.set("name_field", name_);
    }
}


std::ostream& Shapefile::list(std::ostream& out) const {
    JSON j(out);
    j.startObject();

    j << "type" << "shapefile";
    j << "shp" << shpPath_;
    j << "dbf" << dbfPath_;
    j << "size" << size();
    if (!name_.empty()) {
        j << "name" << name_;
        j << "areas";
        j.startList();
        for (const auto& [key, val_] : to_entity) {
            j << key;
        }
        j.endList();
    }

    j.endObject();
    return out;
}


Area* Shapefile::make_area_from_name(const std::string& name) const {
    if (name_.empty()) {
        throw exception::AreaError("Shapefile: areas by 'name' require 'name_field'", Here());
    }

    auto it = to_entity.find(name);
    if (it == to_entity.end()) {
        throw exception::AreaError("Shapefile: '" + name + "' not found (in field '" + name_ + "')", Here());
    }

    if (it->second < 0) {
        throw exception::AreaError("Shapefile: '" + name + "' not unique (in field '" + name_ + "')", Here());
    }

    return make_area(static_cast<size_t>(it->second), name);
}


Area* Shapefile::make_area(size_t entity) const {
    auto it = std::find_if(to_entity.begin(), to_entity.end(),
                           [entity](const auto& kv) { return static_cast<size_t>(kv.second) == entity; });
    return make_area(entity, it != to_entity.end() ? it->first : "");
}


Area* Shapefile::make_area(size_t entity, const std::string& name) const {
    if (entity >= size()) {
        throw exception::AreaError(
            "Shapefile: index " + std::to_string(entity) + " out of range [0, " + std::to_string(size()) + ")", Here());
    }

    struct Object : std::unique_ptr<SHPObject, decltype(&SHPDestroyObject)> {
        explicit Object(SHPObject* ptr) : unique_ptr{ptr, SHPDestroyObject} { ASSERT(operator bool()); }
    };

    Object obj(SHPReadObject(impl_->shp, static_cast<int>(entity)));
    ASSERT(obj);
    ASSERT(obj->nSHPType == SHPT_ARC || obj->nSHPType == SHPT_POLYGON);

    area::Polygon::container_type parts;

    for (int p = 0; p < obj->nParts; ++p) {
        auto start = obj->panPartStart[p];
        auto end   = (p == obj->nParts - 1) ? obj->nVertices : obj->panPartStart[p + 1];

        if (start < end) {
            using points_type = polygon::Polygon::container_type;

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

    // the entity, by name if possible
    spec::Custom spec{{"type", "shapefile"}};
    if (!library_.empty()) {
        spec.set("library", library_);
    }
    else if (url_.empty()) {
        spec.set("file", file_.asString());
    }
    else {
        spec.set("url", url_);
    }

    if (library_.empty() && !dbf_.asString().empty()) {
        spec.set("dbf", dbf_.asString());
    }

    if (!name.empty()) {
        spec.set("name_field", name_);
        spec.set("name", name);
    }
    else {
        spec.set("index", entity);
    }

    return new Entity(std::move(parts), spec);
}


}  // namespace eckit::geo::area::library
