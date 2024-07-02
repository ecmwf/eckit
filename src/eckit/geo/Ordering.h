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


namespace eckit::geo {
class Spec;
}


namespace eckit::geo {


enum Ordering
{
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
    // TODO regular_ ... shift

    healpix_ring,
    healpix_nested,

    scan_ordering        = scan_i_positively_j_negatively_ij_i_single_direction,
    scan_ordering_end    = scan_i_negatively_j_positively_ji_i_alternating_direction,
    healpix_ordering     = healpix_ring,
    healpix_ordering_end = healpix_nested,

    DEFAULT = scan_i_positively_j_negatively_ij_i_single_direction
};


Ordering make_ordering_from_spec(const Spec&);

bool ordering_is_i_positive(Ordering);
bool ordering_is_j_positive(Ordering);
bool ordering_is_ij(Ordering);
bool ordering_is_alternating(Ordering);


}  // namespace eckit::geo
