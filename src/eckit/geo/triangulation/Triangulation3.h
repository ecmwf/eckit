/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/Triangulation.h"


namespace eckit::geo {
class Grid;
}  // namespace eckit::geo


namespace eckit::geo::triangulation {


class Triangulation3 : public Triangulation {
public:
    explicit Triangulation3(const Grid&);
};


}  // namespace eckit::geo::triangulation