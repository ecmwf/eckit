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
