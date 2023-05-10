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

#include "eckit/container/KDTree.h"
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"

namespace eckit::geo {

namespace search {

template <typename PointT, typename PayloadT>
struct Traits {
    using Point   = PointT;
    using Payload = PayloadT;
};

}  // namespace search

using Search3 = KDTreeMemory<search::Traits<geometry::Point3, size_t>>;

using Search2 = KDTreeMemory<search::Traits<geometry::Point2, size_t>>;

}  // namespace eckit::geo
