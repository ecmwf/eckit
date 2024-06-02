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


#include "eckit/geo/Ordering.h"

#include "eckit/geo/Spec.h"


namespace eckit::geo {


Ordering make_ordering_from_spec(const Spec& spec) {
    static const Ordering SCAN[] = {
        scan_i_positively_j_negatively_ij_i_single_direction,
        scan_i_negatively_j_negatively_ij_i_single_direction,
        scan_i_positively_j_positively_ij_i_single_direction,
        scan_i_negatively_j_positively_ij_i_single_direction,
        scan_i_positively_j_negatively_ji_i_single_direction,
        scan_i_negatively_j_negatively_ji_i_single_direction,
        scan_i_positively_j_positively_ji_i_single_direction,
        scan_i_negatively_j_positively_ji_i_single_direction,
        scan_i_positively_j_negatively_ij_i_alternating_direction,
        scan_i_negatively_j_negatively_ij_i_alternating_direction,
        scan_i_positively_j_positively_ij_i_alternating_direction,
        scan_i_negatively_j_positively_ij_i_alternating_direction,
        scan_i_positively_j_negatively_ji_i_alternating_direction,
        scan_i_negatively_j_negatively_ji_i_alternating_direction,
        scan_i_positively_j_positively_ji_i_alternating_direction,
        scan_i_negatively_j_positively_ji_i_alternating_direction,
    };

    int key = (spec.get_bool("scan_i_plus", true) ? 0 : 1) + (spec.get_bool("scan_j_plus", false) ? 1 << 1 : 0)
              + (spec.get_bool("scan_ij", true) ? 0 : 1 << 2) + (spec.get_bool("scan_alternating", false) ? 1 << 3 : 0);

    return SCAN[key];
}


bool ordering_is_i_positive(Ordering o) {
    return o == scan_i_positively_j_negatively_ij_i_single_direction
           || o == scan_i_positively_j_positively_ij_i_single_direction
           || o == scan_i_positively_j_negatively_ji_i_single_direction
           || o == scan_i_positively_j_positively_ji_i_single_direction
           || o == scan_i_positively_j_negatively_ij_i_alternating_direction
           || o == scan_i_positively_j_positively_ij_i_alternating_direction
           || o == scan_i_positively_j_negatively_ji_i_alternating_direction
           || o == scan_i_positively_j_positively_ji_i_alternating_direction;
}


bool ordering_is_j_positive(Ordering o) {
    return o == scan_i_positively_j_positively_ij_i_single_direction
           || o == scan_i_negatively_j_positively_ij_i_single_direction
           || o == scan_i_positively_j_positively_ji_i_single_direction
           || o == scan_i_negatively_j_positively_ji_i_single_direction
           || o == scan_i_positively_j_positively_ij_i_alternating_direction
           || o == scan_i_negatively_j_positively_ij_i_alternating_direction
           || o == scan_i_positively_j_positively_ji_i_alternating_direction
           || o == scan_i_negatively_j_positively_ji_i_alternating_direction;
}


bool ordering_is_ij(Ordering o) {
    return o == scan_i_positively_j_negatively_ij_i_single_direction
           || o == scan_i_negatively_j_negatively_ij_i_single_direction
           || o == scan_i_positively_j_positively_ij_i_single_direction
           || o == scan_i_negatively_j_positively_ij_i_single_direction
           || o == scan_i_positively_j_negatively_ij_i_alternating_direction
           || o == scan_i_negatively_j_negatively_ij_i_alternating_direction
           || o == scan_i_positively_j_positively_ij_i_alternating_direction
           || o == scan_i_negatively_j_positively_ij_i_alternating_direction;
}


bool ordering_is_alternating(Ordering o) {
    return o == scan_i_positively_j_negatively_ij_i_alternating_direction
           || o == scan_i_negatively_j_negatively_ij_i_alternating_direction
           || o == scan_i_positively_j_positively_ij_i_alternating_direction
           || o == scan_i_negatively_j_positively_ij_i_alternating_direction
           || o == scan_i_positively_j_negatively_ji_i_alternating_direction
           || o == scan_i_negatively_j_negatively_ji_i_alternating_direction
           || o == scan_i_positively_j_positively_ji_i_alternating_direction
           || o == scan_i_negatively_j_positively_ji_i_alternating_direction;
}


}  // namespace eckit::geo
