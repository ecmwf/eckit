// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


// Deprecated API, kept for the users of the previous eckit::geo (to remove in a future version)


#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/geo/order/Scan.h"


namespace eckit::geo {


const Grid::Iterator* Grid::Iterator::operator->() const {
    return this;
}


cache::Download::Download(const PathName& root) : download_(root) {}


PathName cache::Download::to_cached_path(const std::string& url) const {
    return download_.toCachedPath(url);
}


bool grid::reduced::ReducedGaussian::octahedral() const {
    return is_reduced_octahedral();
}


order::Scan::Scan(const order_type& order) : Scan(order, mode_t{}) {}


order::Scan::renumber_type order::Scan::reorder(const order_type& to, size_t ni, size_t nj) const {
    return RegularScan{order()}.reorder(to, ni, nj);
}


}  // namespace eckit::geo
