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


#include "eckit/geo/cache/Unzip.h"

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"

#if eckit_HAVE_ZIP
#include <fstream>

#include "eckit/os/AutoUmask.h"
#include "eckit/utils/MD5.h"

#include "zip.h"
#endif


namespace eckit::geo::cache {


#if eckit_HAVE_ZIP


using zip_type = std::unique_ptr<zip_t, decltype(&zip_close)>;


struct unzip_type : std::unique_ptr<zip_file_t, decltype(&zip_fclose)> {
    explicit unzip_type(zip_file_t* ptr) : unique_ptr(ptr, zip_fclose) {}

    void to_path(const PathName& path) {
        // create directory/files with requested permissions, no masking
        AutoUmask umask(0);

        if (auto dir = path.dirName(); !dir.exists()) {
            dir.mkdir();
            if (!dir.exists()) {
                throw WriteError("Unzip: error creating directory '" + dir + "'", Here());
            }
        }

        // decompression buffer
        std::array<char, 4096> buffer{
            0,
        };

        std::ofstream out(path, std::ios::binary);
        for (zip_int64_t r = 0; 0 < (r = zip_fread(get(), buffer.data(), buffer.size()));) {
            out.write(buffer.data(), r);
        }

        if (!path.exists()) {
            throw WriteError("Unzip: error writing to '" + path + "'", Here());
        }
    }
};


Unzip::Unzip(const PathName& root) : root_(root) {}


void Unzip::to_path(const PathName& zip, const PathName& path, const std::string& what) {
    int errorp = 0;
    zip_type z(zip_open(zip.localPath(), ZIP_RDONLY, &errorp), zip_close);
    if (!z) {
        throw CantOpenFile(zip + "', errorp=" + std::to_string(errorp), Here());
    }

    // Extract all files, or a specific one
    if (what.empty()) {
        const auto num_entries = static_cast<zip_uint64_t>(zip_get_num_entries(z.get(), 0));
        for (zip_uint64_t i = 0; i < num_entries; ++i) {
            std::string what = zip_get_name(z.get(), i, 0);

            if (!what.empty() && what.back() != '/') {
                if (unzip_type unz(zip_fopen_index(z.get(), i, 0)); unz) {
                    unz.to_path(path / what);
                }
            }
        }

        return;
    }

    if (unzip_type unz(zip_fopen(z.get(), what.c_str(), 0)); unz) {
        unz.to_path(path);
        return;
    }

    throw ReadError("Unzip: '" + what + "' not found in '" + zip + "'", Here());
}


PathName Unzip::to_cached_path(const PathName& zip, const std::string& what, const std::string& prefix,
                               const std::string& extension) const {
    const auto key  = MD5{zip / what}.digest();
    const auto path = root_ / (zip.baseName() + ".dir") / (prefix + (prefix.empty() ? "" : "-") + key + extension);

    if (!path.exists()) {
        to_path(zip, path, what);
        ASSERT_MSG(path.exists(), "Unzip: file '" + path + "' not found");
    }

    return path;
}


std::vector<std::string> Unzip::list(const PathName& zip, bool files_only) {
    std::vector<std::string> list;

    int errorp = 0;
    zip_type z(zip_open(zip.localPath(), ZIP_RDONLY, &errorp), zip_close);
    if (!z) {
        throw CantOpenFile(zip + "', errorp=" + std::to_string(errorp), Here());
    }

    const auto num_entries = static_cast<zip_uint64_t>(zip_get_num_entries(z.get(), 0));
    list.reserve(num_entries);

    for (zip_uint64_t i = 0; i < num_entries; ++i) {
        if (std::string what = zip_get_name(z.get(), i, 0); !what.empty()) {
            if (!files_only || what.back() != '/') {
                list.emplace_back(what);
            }
        }
    }

    return list;
}


#else


Unzip::Unzip(const PathName& root) : root_(root) {
    NOTIMP;
}


void Unzip::to_path(const PathName&, const PathName&, const std::string&) {
    NOTIMP;
}


PathName Unzip::to_cached_path(const PathName&, const std::string&, const std::string&, const std::string&) const {
    NOTIMP;
}


std::vector<std::string> Unzip::list(const PathName&, bool) {
    NOTIMP;
}


#endif


}  // namespace eckit::geo::cache
