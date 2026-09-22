// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_system_ResourceUsage_H
#define eckit_system_ResourceUsage_H

#include <sys/resource.h>

#include <cstddef>
#include <iosfwd>

namespace eckit::system {

//----------------------------------------------------------------------------------------------------------------------

class ResourceUsage {

public:  // methods

    ResourceUsage();

    size_t maxResidentSetSize() const;
    double cpuTime() const;
    double totalCpuTime() const;
    size_t numberOfSwaps() const;

protected:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const ResourceUsage& p) {
        p.print(s);
        return s;
    }

private:  // members

    size_t factor_;
    struct rusage usage_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system

#endif
