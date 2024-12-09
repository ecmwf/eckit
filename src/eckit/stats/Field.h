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
#include <vector>


namespace eckit::stats {


using MIRValuesVector = std::vector<double>;


class Field {
public:
    size_t dimensions() const;
    const MIRValuesVector& values(size_t which) const;
    void update(MIRValuesVector&, size_t which, bool recomputeHasMissing = false);

    bool hasMissing() const;
    double missingValue() const;

    size_t truncation() const;
};


}  // namespace eckit::stats
