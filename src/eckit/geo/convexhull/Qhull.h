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

#include "eckit/geo/ConvexHull.h"

#include <memory>
#include <string>
#include <vector>


namespace orgQhull {
class Qhull;
}  // namespace orgQhull


namespace eckit::geo::convexhull {


class Qhull : public ConvexHull {
protected:
    Qhull(size_t N, const std::vector<double>&, const std::string& command);

    using qh_type = orgQhull::Qhull;

    const qh_type& qh();

private:
    std::unique_ptr<qh_type> qh_;
};


}  // namespace eckit::geo::convexhull
