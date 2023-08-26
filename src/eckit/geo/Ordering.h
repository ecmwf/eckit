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


enum Ordering
{
    regular_scan_i_positively_j_negatively_ij_i_single_direction,
    regular_scan_i_negatively_j_negatively_ij_i_single_direction,
    regular_scan_i_positively_j_positively_ij_i_single_direction,
    regular_scan_i_negatively_j_positively_ij_i_single_direction,
    regular_scan_i_positively_j_negatively_ji_i_single_direction,
    regular_scan_i_negatively_j_negatively_ji_i_single_direction,
    regular_scan_i_positively_j_positively_ji_i_single_direction,
    regular_scan_i_negatively_j_positively_ji_i_single_direction,
    regular_scan_i_positively_j_negatively_ij_i_alternating_direction,
    regular_scan_i_negatively_j_negatively_ij_i_alternating_direction,
    regular_scan_i_positively_j_positively_ij_i_alternating_direction,
    regular_scan_i_negatively_j_positively_ij_i_alternating_direction,
    regular_scan_i_positively_j_negatively_ji_i_alternating_direction,
    regular_scan_i_negatively_j_negatively_ji_i_alternating_direction,
    regular_scan_i_positively_j_positively_ji_i_alternating_direction,
    regular_scan_i_negatively_j_positively_ji_i_alternating_direction,
    // TODO regular_scan_ ... shift

    reduced_scan_i_positively_j_negatively,
    reduced_scan_i_negatively_j_negatively,
    reduced_scan_i_positively_j_positively,
    reduced_scan_i_negatively_j_positively,
    // TODO reduced_scan_ ... shift

    healpix_ring,
    healpix_nested,
};


}  // namespace eckit::geo
