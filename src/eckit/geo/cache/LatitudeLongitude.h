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

#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/RecordCache.h"


namespace eckit::geo::cache {


class LatitudeLongitude : public RecordCache {
public:

    static const LatitudeLongitude& get(const Grid::uid_type&);

    bytes_size_t footprint() const override { return 2 * size() * sizeof(double); }
    void read(const PathName&) override;

    size_t size() const;
    const std::vector<double>& longitude() const { return longitude_; }
    const std::vector<double>& latitude() const { return latitude_; }

private:

    std::vector<double> longitude_;
    std::vector<double> latitude_;
};


}  // namespace eckit::geo::cache
