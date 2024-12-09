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


#include "eckit/stats/Field.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::stats {


size_t Field::dimensions() const {
    NOTIMP;
}


const MIRValuesVector& Field::values(size_t which) const {
    NOTIMP;
}


void Field::update(MIRValuesVector&, size_t which, bool recomputeHasMissing) {
    NOTIMP;
}


bool Field::hasMissing() const {
    NOTIMP;
}


double Field::missingValue() const {
    NOTIMP;
}


size_t Field::truncation() const {
    NOTIMP;
}


}  // namespace eckit::stats
