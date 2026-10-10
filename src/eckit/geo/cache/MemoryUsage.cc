// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/MemoryUsage.h"

#include <ostream>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::cache {


MemoryUsage::MemoryUsage(const std::string& str) {
    const auto v = StringTools::split(",", str);
    if (v.empty() || v.size() > 2) {
        throw BadValue("MemoryUsage: invalid '" + str + "', expected 'memory[,shared]'", Here());
    }

    Translator<std::string, size_t> to_size;
    memory_ = to_size(v.front());
    shared_ = to_size(v.back());
}


MemoryUsage& MemoryUsage::operator+=(const MemoryUsage& other) {
    memory_ += other.memory_;
    shared_ += other.shared_;
    return *this;
}


MemoryUsage& MemoryUsage::operator-=(const MemoryUsage& other) {
    memory_ = memory_ > other.memory_ ? memory_ - other.memory_ : 0;
    shared_ = shared_ > other.shared_ ? shared_ - other.shared_ : 0;
    return *this;
}


bool MemoryUsage::exceeds(const MemoryUsage& capacity) const {
    return memory_ > capacity.memory_ || shared_ > capacity.shared_;
}


std::string MemoryUsage::str() const {
    return std::to_string(memory_) + "," + std::to_string(shared_);
}


std::ostream& operator<<(std::ostream& out, const MemoryUsage& usage) {
    return out << "MemoryUsage[memory=" << usage.memory_ << ",shared=" << usage.shared_ << "]";
}


}  // namespace eckit::geo::cache
