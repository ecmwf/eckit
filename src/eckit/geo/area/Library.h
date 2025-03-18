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


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>


namespace eckit::geo {
class Area;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo::area {


class Library {
public:

    // -- Constructors

    Library(const Library&) = delete;
    Library(Library&&)      = delete;

    // -- Destructor

    virtual ~Library() = default;

    // -- Operators

    void operator=(const Library&) = delete;
    void operator=(Library&&)      = delete;

    // -- Methods

    virtual std::ostream& list(std::ostream&) const = 0;
    virtual size_t size() const                     = 0;

    [[nodiscard]] virtual Area* make_area(size_t) const                       = 0;
    [[nodiscard]] virtual Area* make_area_from_name(const std::string&) const = 0;

    virtual void fill_spec(spec::Custom&) const = 0;

protected:

    // -- Constructors

    Library() = default;
};


}  // namespace eckit::geo::area
