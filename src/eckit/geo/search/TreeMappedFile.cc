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


#include "eckit/geo/search/TreeMappedFile.h"

#include "eckit/filesystem/PathExpander.h"
#include "eckit/utils/Tokenizer.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/log/Log.h"


namespace eckit::geo::search {


template <class T>
PathName TreeMappedFile<T>::treePath(const Grid& r, bool makeUnique) {

    // LocalPathName::unique and LocalPathName::mkdir call mkdir, make sure to use umask = 0
    AutoUmask umask(0);

    static const long VERSION = 2;

    const std::string relative = "eckit/geo/search/" + std::to_string(VERSION) + "/" + r.uid() + ".kdtree";

    auto writable = [](const PathName& path) -> bool { return (::access(path.asString().c_str(), W_OK) == 0); };

    for (PathName path : T::roots()) {
        if (not path.exists()) {
            if (not writable(path.dirName())) {
                continue;
            }

            try {
                path.mkdir(0777);
            }
            catch (FailedSystemCall&) {
                // ignore
            }
        }

        if (not writable(path)) {
            Log::debug() << "TreeMappedFile: path '" << path << "' isn't writable" << std::endl;
            continue;
        }

        path /= relative;
        if (makeUnique && !path.exists()) {
            path = PathName::unique(path);
        }

        Log::debug() << "TreeMappedFile: path '" << path << "'" << (makeUnique ? " (unique)" : "") << std::endl;
        return path;
    }

    throw exception::SeriousBug("TreeMappedFile: no paths are viable for caching");
}


template <class T>
PathName TreeMappedFile<T>::lockFile(const std::string& path) {
    AutoUmask umask(0);

    PathName lock(path + ".lock");
    lock.touch();
    return lock;
}


class TreeMappedCacheFile : public TreeMappedFile<TreeMappedCacheFile> {
    using P = TreeMappedFile<TreeMappedCacheFile>;

public:

    using P::P;
    static std::vector<std::string> roots() {
        static auto roots = []() {
            std::vector<std::string> r;
            Tokenizer{":"}(LibEcKitGeo::cacheDir(), r);

            for (auto& root : r) {
                root = PathExpander::expand(root);
            }

            return r;
        }();
        return roots;
    }
};


static const TreeBuilder<TreeMappedCacheFile> builder1("mapped-cache-file");


class TreeMappedTempFile : public TreeMappedFile<TreeMappedTempFile> {
    using P = TreeMappedFile<TreeMappedTempFile>;

public:

    using P::P;
    static std::vector<std::string> roots() {
        static std::vector<std::string> _root{"/tmp"};
        return _root;
    }
};


static const TreeBuilder<TreeMappedTempFile> builder2("mapped-temporary-file");


}  // namespace eckit::geo::search
