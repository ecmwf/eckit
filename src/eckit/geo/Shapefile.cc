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
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/Unzip.h"
#include "eckit/geo/spec/Custom.h"

#include "shapefil.h"


namespace eckit::geo {


namespace {


void shapefile_polygon(const PathName& path_shp, const std::string& /*unused*/) {
    if (path_shp.extension() != ".shp") {
        throw exception::AreaError("Shapefile: path must have .shp extension.", Here());
    }

    auto path = path_shp.asString();
    auto* h   = SHPOpen(path.c_str(), "rb");
    if (h == nullptr) {
        throw exception::AreaError("Shapefile: error opening shapefile.", Here());
    }

    // Get number of shapes
    int nEntities  = 0;
    int nShapeType = 0;
    std::vector<double> adfMinBound(4);
    std::vector<double> adfMaxBound(4);

    SHPGetInfo(h, &nEntities, &nShapeType, adfMinBound.data(), adfMaxBound.data());

    for (int e = 0; e < nEntities; ++e) {
        std::unique_ptr<SHPObject, decltype(&SHPDestroyObject)> shp(SHPReadObject(h, e), SHPDestroyObject);

        if (shp) {
            if (shp->nSHPType == SHPT_POLYGON) {
                // Process polygon
                int nVertices = shp->nVertices;
            }
        }
    }

    SHPClose(h);

    // ...
}


void shapefile_string_attributes(DBFInfo* hDBF) {
    int fieldCount  = DBFGetFieldCount(hDBF);
    int recordCount = DBFGetRecordCount(hDBF);

    for (int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex) {
        char fieldName[12];
        int width    = 0;
        int decimals = 0;

        // Only process string fields
        if (DBFGetFieldInfo(hDBF, fieldIndex, fieldName, &width, &decimals) == FTString) {
            std::cout << "Field \"" << fieldName << "\"" << std::endl;
        }
    }
    std::cout << std::endl;
}


}  // namespace


Shapefile::Shapefile(const Spec& spec) : Shapefile(spec.get_string("file")) {}


Shapefile::Shapefile(const PathName& file) :
    shp_(file.extension() == ".zip" ? [](const PathName& zip) -> PathName {
        static cache::Unzip CACHE_UNZIP(LibEcKitGeo::cacheDir() + "/shapefile");

        auto dir = CACHE_UNZIP.to_cached_path(zip);

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
    }(file)
                                     : file), dbf_(shp_.asString().substr(0, shp_.asString().length() - 4) + ".dbf") {
    SHPInfo* hSHP = nullptr;
    if (!shp_.exists() || shp_.extension() != ".shp" || (hSHP = SHPOpen(shp_.localPath(), "rb")) == nullptr) {
        throw CantOpenFile(shp_ + ", expecting .shp", Here());
    }

    DBFInfo* hDBF = nullptr;
    if (!dbf_.exists() || dbf_.extension() != ".dbf" || (hDBF = DBFOpen(dbf_.localPath(), "rb")) == nullptr) {
        throw CantOpenFile(dbf_ + ", expecting .dbf", Here());
    }

    // Find the field index for the "NAME" attribute (the field name might change!)
    shapefile_string_attributes(hDBF);
    int nameField = DBFGetFieldIndex(hDBF, "NAME");

    // Get number of shapes
    int nEntities  = 0;
    int nShapeType = 0;
    std::vector<double> adfMinBound(4);
    std::vector<double> adfMaxBound(4);

    SHPGetInfo(hSHP, &nEntities, &nShapeType, adfMinBound.data(), adfMaxBound.data());

    for (int e = 0; e < nEntities; ++e) {
        std::unique_ptr<SHPObject, decltype(&SHPDestroyObject)> shp(SHPReadObject(hSHP, e), SHPDestroyObject);

        if (shp) {
            if (shp->nSHPType == SHPT_ARC || shp->nSHPType == SHPT_POLYGON) {
                std::string areaName;
                if (nameField >= 0) {
                    // Read the string attribute from the DBF file for this record
                    const auto* nameAttr = DBFReadStringAttribute(hDBF, e, nameField);
                    if (nameAttr != nullptr) {
                        areaName = nameAttr;
                    }
                }
                // std::cout << "Polygon " << e << " with " << shp->nVertices << " vertices";
                // if (!areaName.empty()) {
                //     std::cout << " and name: " << areaName;
                // }
                // std::cout << std::endl;
            }
        }
    }

    SHPClose(hSHP);
    DBFClose(hDBF);
}


Shapefile* Shapefile::make_from_url(const std::string& url) {
    static cache::Download download(LibEcKitGeo::cacheDir() + "/area");

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
        cache::Unzip CACHE_UNZIP(LibEcKitGeo::cacheDir() + "/shapefile");
        CACHE_UNZIP.to_cached_path(zip);
    }

    auto path_shp = dir / whats.front();
    ASSERT(path_shp.exists());
    return new Shapefile(path_shp);
}


std::ostream& Shapefile::list(std::ostream& out) const {
    NOTIMP;
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
