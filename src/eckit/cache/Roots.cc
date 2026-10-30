// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Roots.h"

#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iterator>
#include <ostream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathExpander.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit::cache {


Roots::Roots(const std::string& roots, bool writing) : writing_(writing) {
    std::vector<std::string> v;
    Tokenizer(":")(roots, v);

    for (const auto& root : v) {
        if (!root.empty()) {
            // see eckit::PathExpander, and "~" as the user's home
            paths_.emplace_back(PathExpander::expand(root), true);
        }
    }
}


Roots::Roots(const PathName& root, bool writing) : paths_{root}, writing_(writing) {}


Roots::Roots(std::vector<PathName> roots, bool writing) : paths_(std::move(roots)), writing_(writing) {}


Roots Roots::operator/(const std::string& sub) const {
    std::vector<PathName> paths;
    paths.reserve(paths_.size());
    for (const auto& p : paths_) {
        paths.emplace_back(p / sub);
    }
    return Roots(std::move(paths), writing_);
}


std::vector<PathName> Roots::readable_paths() const {
    std::vector<PathName> readable;
    std::copy_if(paths_.begin(), paths_.end(), std::back_inserter(readable),
                 [](const auto& root) { return root.exists() && ::access(root.localPath(), R_OK | X_OK) == 0; });
    return readable;
}


std::vector<PathName> Roots::writable_paths() const {
    std::vector<PathName> writable;
    if (!writing_) {
        return writable;
    }

    // writable, or creatable: the closest existing ancestor is writable
    std::copy_if(paths_.begin(), paths_.end(), std::back_inserter(writable), [](const auto& root) {
        auto p = root;
        while (!p.exists() && p.dirName() != p) {
            p = p.dirName();
        }
        return ::access(p.localPath(), W_OK | X_OK) == 0;
    });
    return writable;
}


bool Roots::find(const PathName& relative, PathName& path, const Valid& valid) const {
    for (const auto& root : paths_) {
        if (auto p = root / relative; p.exists() && (!valid || valid(p))) {
            path = p;
            return true;
        }
    }
    return false;
}


void Roots::create_directory(const PathName& root, const PathName& relative) {
    // the umask is not changed (it is process-wide), the mode is set explicitly
    auto shared = [](const PathName& dir) {
        if (dir.exists()) {
            return;
        }

        try {
            dir.mkdir();
        }
        catch (const FailedSystemCall&) {
            if (!dir.isDir()) {
                throw;
            }
        }

        ::chmod(dir.localPath(), 0777);  // not if created meanwhile by another user (only its owner can)
    };

    if (!root.exists()) {
        root.dirName().mkdir();
        shared(root);
    }

    auto dir = root;
    for (const auto& part : StringTools::split("/", relative.asString())) {
        dir = dir / part;
        shared(dir);
    }
}


std::ostream& operator<<(std::ostream& out, const Roots& roots) {
    out << "Roots[";
    const char* sep = "";
    for (const auto& p : roots.paths_) {
        out << sep << p;
        sep = ":";
    }
    return out << "]";
}


}  // namespace eckit::cache
