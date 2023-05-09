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

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef ASSERT_MSG
#undef ASSERT_MSG
#endif

#ifdef NOTIMP
#undef NOTIMP
#endif

#include "grit/types.h"


namespace grit {


namespace search {


template <typename PointT, typename PayloadT>
struct Traits {
    using Point   = PointT;
    using Payload = PayloadT;
};


}  // namespace search


using SearchXYZ = eckit::KDTreeMemory<search::Traits<PointXYZ, size_t>>;


using SearchXY = eckit::KDTreeMemory<search::Traits<PointXY, size_t>>;


}  // namespace grit
