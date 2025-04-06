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


#include "eckit/geo/order/Scan.h"


namespace eckit::geo::order {


const Order::value_type Scan::scan_i_positively_j_negatively_ij = "scan_i_positively_j_negatively_ij";
const Order::value_type Scan::scan_i_negatively_j_negatively_ij = "scan_i_negatively_j_negatively_ij";
const Order::value_type Scan::scan_i_positively_j_positively_ij = "scan_i_positively_j_positively_ij";
const Order::value_type Scan::scan_i_negatively_j_positively_ij = "scan_i_negatively_j_positively_ij";
const Order::value_type Scan::scan_i_positively_j_negatively_ji = "scan_i_positively_j_negatively_ji";
const Order::value_type Scan::scan_i_negatively_j_negatively_ji = "scan_i_negatively_j_negatively_ji";
const Order::value_type Scan::scan_i_positively_j_positively_ji = "scan_i_positively_j_positively_ji";
const Order::value_type Scan::scan_i_negatively_j_positively_ji = "scan_i_negatively_j_positively_ji";
const Order::value_type Scan::scan_i_positively_j_negatively_ij_alternating =
    "scan_i_positively_j_negatively_ij_alternating";
const Order::value_type Scan::scan_i_negatively_j_negatively_ij_alternating =
    "scan_i_negatively_j_negatively_ij_alternating";
const Order::value_type Scan::scan_i_positively_j_positively_ij_alternating =
    "scan_i_positively_j_positively_ij_alternating";
const Order::value_type Scan::scan_i_negatively_j_positively_ij_alternating =
    "scan_i_negatively_j_positively_ij_alternating";
const Order::value_type Scan::scan_i_positively_j_negatively_ji_alternating =
    "scan_i_positively_j_negatively_ji_alternating";
const Order::value_type Scan::scan_i_negatively_j_negatively_ji_alternating =
    "scan_i_negatively_j_negatively_ji_alternating";
const Order::value_type Scan::scan_i_positively_j_positively_ji_alternating =
    "scan_i_positively_j_positively_ji_alternating";
const Order::value_type Scan::scan_i_negatively_j_positively_ji_alternating =
    "scan_i_negatively_j_positively_ji_alternating";

Scan::Scan() :
    Order(scan_i_positively_j_negatively_ij,              //
          scan_i_negatively_j_negatively_ij,              //
          scan_i_positively_j_positively_ij,              //
          scan_i_negatively_j_positively_ij,              //
          scan_i_positively_j_negatively_ji,              //
          scan_i_negatively_j_negatively_ji,              //
          scan_i_positively_j_positively_ji,              //
          scan_i_negatively_j_positively_ji,              //
          scan_i_positively_j_negatively_ij_alternating,  //
          scan_i_negatively_j_negatively_ij_alternating,  //
          scan_i_positively_j_positively_ij_alternating,  //
          scan_i_negatively_j_positively_ij_alternating,  //
          scan_i_positively_j_negatively_ji_alternating,  //
          scan_i_negatively_j_negatively_ji_alternating,  //
          scan_i_positively_j_positively_ji_alternating,  //
          scan_i_negatively_j_positively_ji_alternating) {}

bool Scan::is_scan_i_positive(const value_type& o) const {
    return o == scan_i_positively_j_negatively_ij || o == scan_i_positively_j_positively_ij ||
           o == scan_i_positively_j_negatively_ji || o == scan_i_positively_j_positively_ji ||
           o == scan_i_positively_j_negatively_ij_alternating || o == scan_i_positively_j_positively_ij_alternating ||
           o == scan_i_positively_j_negatively_ji_alternating || o == scan_i_positively_j_positively_ji_alternating;
}


bool Scan::is_scan_j_positive(const value_type& o) const {
    return o == scan_i_positively_j_positively_ij || o == scan_i_negatively_j_positively_ij ||
           o == scan_i_positively_j_positively_ji || o == scan_i_negatively_j_positively_ji ||
           o == scan_i_positively_j_positively_ij_alternating || o == scan_i_negatively_j_positively_ij_alternating ||
           o == scan_i_positively_j_positively_ji_alternating || o == scan_i_negatively_j_positively_ji_alternating;
}


bool Scan::is_scan_ij(const value_type& o) const {
    return o == scan_i_positively_j_negatively_ij || o == scan_i_negatively_j_negatively_ij ||
           o == scan_i_positively_j_positively_ij || o == scan_i_negatively_j_positively_ij ||
           o == scan_i_positively_j_negatively_ij_alternating || o == scan_i_negatively_j_negatively_ij_alternating ||
           o == scan_i_positively_j_positively_ij_alternating || o == scan_i_negatively_j_positively_ij_alternating;
}


bool Scan::is_scan_alternating_direction(const value_type& o) const {
    return o == scan_i_positively_j_negatively_ij_alternating || o == scan_i_negatively_j_negatively_ij_alternating ||
           o == scan_i_positively_j_positively_ij_alternating || o == scan_i_negatively_j_positively_ij_alternating ||
           o == scan_i_positively_j_negatively_ji_alternating || o == scan_i_negatively_j_negatively_ji_alternating ||
           o == scan_i_positively_j_positively_ji_alternating || o == scan_i_negatively_j_positively_ji_alternating;
}


}  // namespace eckit::geo::order
