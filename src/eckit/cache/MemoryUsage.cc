// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/MemoryUsage.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"


namespace eckit::cache {


MemoryUsage::MemoryUsage(const std::string& str) {
    const auto v = StringTools::split(",", str);
    if (v.empty() || v.size() > 2) {
        throw BadValue("MemoryUsage: invalid '" + str + "', expected 'process[,shared]'", Here());
    }

    Translator<std::string, size_t> to_size;
    process_ = to_size(v.front());
    shared_  = v.size() == 2 ? to_size(v.back()) : 0;
}


MemoryUsage& MemoryUsage::operator+=(const MemoryUsage& other) {
    process_ += other.process_;
    shared_ += other.shared_;
    return *this;
}


MemoryUsage& MemoryUsage::operator-=(const MemoryUsage& other) {
    process_ = process_ > other.process_ ? process_ - other.process_ : 0;
    shared_  = shared_ > other.shared_ ? shared_ - other.shared_ : 0;
    return *this;
}


bool MemoryUsage::exceeds(const MemoryUsage& capacity) const {
    return process_ > capacity.process_ || shared_ > capacity.shared_;
}


std::ostream& operator<<(std::ostream& out, const MemoryUsage& usage) {
    return out << "MemoryUsage[process=" << usage.process_ << ",shared=" << usage.shared_ << "]";
}


}  // namespace eckit::cache
