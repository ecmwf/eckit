/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @author Peter Bispham
/// @date Apr 2015


#include "mir/search/tree/TreeMappedFile.h"

#include "eckit/parser/Tokenizer.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace search {
namespace tree {


// -----------------------------------------------------------------------------


template<class T>
eckit::PathName TreeMappedFile<T>::treePath(const repres::Representation& r, bool makeUnique) {

    // LocalPathName::unique and LocalPathName::mkdir call mkdir, make sure to use umask = 0
    eckit::AutoUmask umask(0);

    static const long VERSION = 1;

    const std::string relative = "mir/search/"
            + std::to_string(VERSION)
            + "/"
            + r.uniqueName()
            + ".kdtree";

    auto writable = [](const eckit::PathName& path) -> bool {
        return (::access(path.asString().c_str(), W_OK) == 0);
    };

    for (const eckit::PathName root : T::roots()) {

        // mkdir if not exists
        try {
            root.mkdir();
        } catch (eckit::FailedSystemCall&) {
            // ...
        }

        if (not writable(root)) {
            eckit::Log::debug<LibMir>() << "TreeMappedFile: path '" << root << "' isn't writable" << std::endl;
            continue;
        }

        eckit::PathName p = root + "/" + relative;
        if (makeUnique && !p.exists()) {
            p = eckit::PathName::unique(p);
        }

        eckit::Log::debug<LibMir>() << "TreeMappedFile: path '" << p  << "'" << (makeUnique ? " (unique)" : "") << std::endl;
        return p;
    }

    throw eckit::SeriousBug("TreeMappedFile: no paths are viable for caching");
}


template<class T>
eckit::PathName TreeMappedFile<T>::lockFile(const std::string& path) {
    eckit::AutoUmask umask(0);

    eckit::PathName lock(path + ".lock");
    lock.touch();
    return lock;
}


// -----------------------------------------------------------------------------


class TreeMappedCacheFile : public TreeMappedFile<TreeMappedCacheFile> {
    using P = TreeMappedFile<TreeMappedCacheFile>;
    static std::vector<std::string> getRoots() {
        static std::string cacheDir = LibMir::cacheDir();

        std::vector<std::string> roots;
        eckit::Tokenizer parse(":");

        parse(cacheDir, roots);
        return roots;
    }
public:
    using P::P;
    static std::vector<std::string> roots() {
        static std::vector<std::string> roots = getRoots();
        return roots;
    }
};


static TreeBuilder<TreeMappedCacheFile> builder1("tree-mapped-cache-file");


// -----------------------------------------------------------------------------


class TreeMappedTempFile : public TreeMappedFile<TreeMappedTempFile> {
    using P = TreeMappedFile<TreeMappedTempFile>;
public:
    using P::P;
    static std::vector<std::string> roots() {
        static std::vector<std::string> _root {"/tmp"};
        return _root;
    }
};


static TreeBuilder<TreeMappedTempFile> builder2("tree-mapped-temporary-file");

}  // namespace tree
}  // namespace search
}  // namespace mir

