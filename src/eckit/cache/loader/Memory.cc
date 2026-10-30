// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/Memory.h"

#include <ostream>


namespace eckit::cache::loader {


static const LoaderBuilder<Memory> BUILDER("memory");


Memory::Memory(const PathName& path, const Options&) : Loader(path) {}


void Memory::print(std::ostream& out) const {
    out << "Memory[]";
}


}  // namespace eckit::cache::loader
