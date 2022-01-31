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


#include "mir/search/tree/TreeMappedFile.h"

#include "eckit/filesystem/PathExpander.h"
#include "eckit/utils/Tokenizer.h"

#include "mir/config/LibMir.h"
#include "mir/repres/Representation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace search {
namespace tree {


template <class T>
eckit::PathName TreeMappedFile<T>::treePath(const repres::Representation& r, bool makeUnique) {

    // LocalPathName::unique and LocalPathName::mkdir call mkdir, make sure to use umask = 0
    eckit::AutoUmask umask(0);

    static const long VERSION = 2;

    const std::string relative = "mir/search/" + std::to_string(VERSION) + "/" + r.uniqueName() + ".kdtree";

    auto writable = [](const eckit::PathName& path) -> bool { return (::access(path.asString().c_str(), W_OK) == 0); };

    for (eckit::PathName path : T::roots()) {

        if (not path.exists()) {

            if (not writable(path.dirName())) {
                continue;
            }

            try {
                path.mkdir(0777);
            }
            catch (eckit::FailedSystemCall&) {
                // ignore
            }
        }

        if (not writable(path)) {
            Log::debug() << "TreeMappedFile: path '" << path << "' isn't writable" << std::endl;
            continue;
        }

        path /= relative;
        if (makeUnique && !path.exists()) {
            path = eckit::PathName::unique(path);
        }

        Log::debug() << "TreeMappedFile: path '" << path << "'" << (makeUnique ? " (unique)" : "") << std::endl;
        return path;
    }

    throw exception::SeriousBug("TreeMappedFile: no paths are viable for caching");
}


template <class T>
eckit::PathName TreeMappedFile<T>::lockFile(const std::string& path) {
    eckit::AutoUmask umask(0);

    eckit::PathName lock(path + ".lock");
    lock.touch();
    return lock;
}


class TreeMappedCacheFile : public TreeMappedFile<TreeMappedCacheFile> {
    using P = TreeMappedFile<TreeMappedCacheFile>;
    static std::vector<std::string> getRoots() {
        static std::string cacheDir = LibMir::cacheDir();

        std::vector<std::string> tmp;
        eckit::Tokenizer parse(":");
        parse(cacheDir, tmp);

        std::vector<std::string> r;
        r.reserve(tmp.size());

        for (const auto& root : tmp) {
            r.emplace_back(eckit::PathExpander::expand(root));
        }

        return r;
    }

public:
    using P::P;
    static std::vector<std::string> roots() {
        static std::vector<std::string> roots = getRoots();
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


}  // namespace tree
}  // namespace search
}  // namespace mir
