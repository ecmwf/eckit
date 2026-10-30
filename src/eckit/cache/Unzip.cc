// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Unzip.h"

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"

#if eckit_HAVE_ZIP
#include <algorithm>
#include <array>
#include <fstream>
#include <ios>
#include <memory>
#include <utility>

#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"

#include "zip.h"
#endif


namespace eckit::cache {


#if eckit_HAVE_ZIP


namespace {


using zip_type = std::unique_ptr<zip_t, decltype(&zip_close)>;


zip_type open_zip(const PathName& zip) {
    int error = 0;
    zip_type z(zip_open(zip.localPath(), ZIP_RDONLY, &error), zip_close);
    if (!z) {
        throw CantOpenFile(zip + " (libzip error " + std::to_string(error) + ")", false, Here());
    }
    return z;
}


std::string entry_name(zip_t* z, zip_uint64_t index, const PathName& zip) {
    const char* name = zip_get_name(z, index, 0);
    if (name == nullptr) {
        throw ReadError("Unzip: entry #" + std::to_string(index) + " of '" + zip + "' has no name", Here());
    }
    return name;
}


/// If an entry stays inside the extraction directory: no absolute path or ".." ("zip slip")
bool is_relative_and_inside(const std::string& name) {
    if (name.empty() || name.front() == '/') {
        return false;
    }

    const auto parts = StringTools::split("/", name);
    return std::none_of(parts.begin(), parts.end(), [](const auto& part) { return part == ".."; });
}


struct unzip_type : std::unique_ptr<zip_file_t, decltype(&zip_fclose)> {
    explicit unzip_type(zip_file_t* ptr) : unique_ptr(ptr, zip_fclose) {}

    void to_path(const PathName& path) {
        if (auto dir = path.dirName(); !dir.exists()) {
            dir.mkdir();
            if (!dir.exists()) {
                throw WriteError("Unzip: error creating directory '" + dir + "'", Here());
            }
        }

        std::array<char, 4096> buffer{
            0,
        };

        std::ofstream out(path.localPath(), std::ios::binary);

        // a corrupt entry (e.g. failing its checksum) is an error, not a partial file
        zip_int64_t r = 0;
        while (0 < (r = zip_fread(get(), buffer.data(), buffer.size()))) {
            out.write(buffer.data(), static_cast<std::streamsize>(r));
        }

        if (r < 0) {
            throw ReadError("Unzip: error extracting to '" + path + "'", Here());
        }

        out.close();
        if (!out) {
            throw WriteError("Unzip: error writing to '" + path + "'", Here());
        }
    }
};


}  // namespace


void Unzip::to_path(const PathName& zip, const PathName& path, const std::string& what) {
    auto z = open_zip(zip);

    if (what.empty()) {
        if (!path.exists()) {
            path.mkdir();  // even without files
        }

        const auto num_entries = static_cast<zip_uint64_t>(zip_get_num_entries(z.get(), 0));
        for (zip_uint64_t i = 0; i < num_entries; ++i) {
            const auto name = entry_name(z.get(), i, zip);
            if (!is_relative_and_inside(name)) {
                throw ReadError("Unzip: entry '" + name + "' of '" + zip + "' is outside of the extraction", Here());
            }

            if (name.back() == '/') {
                continue;  // directories are created with their files
            }

            unzip_type unz(zip_fopen_index(z.get(), i, 0));
            if (!unz) {
                throw ReadError("Unzip: cannot open entry '" + name + "' of '" + zip + "'", Here());
            }

            unz.to_path(path / name);
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
                               const std::string& suffix) const {
    // the archive as it is: real path and version (size, modification time)
    const auto real = zip.realName();
    MD5 md5;
    md5.add(real.asString());
    md5.add(static_cast<long long>(real.size()));
    md5.add(static_cast<long long>(real.lastModified()));
    md5.add(what);
    const auto key = md5.digest();

    const auto relative = PathName{zip.baseName() + ".dir"} / (prefix + (prefix.empty() ? "" : "-") + key + suffix);
    return get_or_create(relative, [&](const PathName& tmp) { to_path(zip, tmp, what); });
}


std::vector<std::string> Unzip::list(const PathName& zip, bool files_only) {
    auto z = open_zip(zip);

    const auto num_entries = static_cast<zip_uint64_t>(zip_get_num_entries(z.get(), 0));

    std::vector<std::string> list;
    list.reserve(num_entries);

    for (zip_uint64_t i = 0; i < num_entries; ++i) {
        if (auto name = entry_name(z.get(), i, zip); !name.empty() && (!files_only || name.back() != '/')) {
            list.emplace_back(std::move(name));
        }
    }

    return list;
}


#else


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


}  // namespace eckit::cache
