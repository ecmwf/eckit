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


#include "mir/stats/ValueStatistics.h"

#include "mir/util/Exceptions.h"


namespace mir {
namespace stats {


void ValueStatistics::operator()(const double&) {
    NOTIMP;  // ensure specialisation
}


void ValueStatistics::operator()(const float&) {
    NOTIMP;  // ensure specialisation
}


}  // namespace stats
}  // namespace mir
