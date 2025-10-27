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

#include <memory>
#include <vector>


namespace eckit {
namespace geo {
class Spec;
}
class PathName;
}  // namespace eckit


namespace eckit::geo::share {


class Area final {
public:

    static const Area& instance();

private:

    // -- Constructors

    explicit Area(const std::vector<PathName>&);

    // -- Members

    std::unique_ptr<Spec> spec_;

    // -- Methods

    void load(const PathName&);
};


}  // namespace eckit::geo::share
