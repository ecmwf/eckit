// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/TmpDir.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static PathName tmp(const char* base) {

    static std::string static_base(eckit::Resource<std::string>("$TMPDIR", "/tmp"));

    // n.b. mutable for mkdtemp
    std::string tmpl = (base ? base : static_base) + "/tmpdir.XXXXXX";
    ASSERT(::mkdtemp(&tmpl[0]) == &tmpl[0]);

    return tmpl;
}


static void deldir(eckit::PathName& p) {

    std::vector<eckit::PathName> files;
    std::vector<eckit::PathName> dirs;
    p.children(files, dirs);

    for (auto& f : files) {
        f.unlink();
    }
    for (auto& d : dirs) {
        deldir(d);
    }

    p.rmdir();
}

TmpDir::TmpDir(const char* base) : PathName(tmp(base)) {}

TmpDir::TmpDir(TmpDir&& rhs) : PathName(rhs) {

    static_cast<PathName&>(rhs) = PathName("");
}

TmpDir::~TmpDir() noexcept(false) {
    if (*this != "") {
        deldir(*this);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
