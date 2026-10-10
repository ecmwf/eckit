// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMappedAnonymousMemory.h"

#include <ostream>


namespace eckit::geo::search {


static const TreeRegisterType<TreeMappedAnonymousMemory> BUILDER("mapped-anonymous-memory");


TreeMappedAnonymousMemory::TreeMappedAnonymousMemory(const std::string& uid, size_t size) : TreeMapped(uid, size) {
#if defined(__linux__)
    // /dev/zero always exists, so storage is created explicitly (it would otherwise be read as an existing tree)
    open("/dev/zero", true);
#else
    // mapping /dev/zero is not supported (e.g. macOS), map an unnamed (unlinked) temporary file instead
    const auto path = PathName::unique("/tmp/eckit-geo-search-anonymous");
    try {
        open(path, true);
    }
    catch (...) {
        if (path.exists()) {
            path.unlink(false);
        }
        throw;
    }
    path.unlink(false);
#endif
}


void TreeMappedAnonymousMemory::print(std::ostream& out) const {
    out << "TreeMappedAnonymousMemory[size=" << size() << "]";
}


}  // namespace eckit::geo::search
