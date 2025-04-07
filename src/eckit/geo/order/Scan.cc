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


#define IP "i_positively"
#define IN "i_negatively"
#define JP "j_positively"
#define JN "j_negatively"
#define AL "_alternating"
#define S "scan_"
#define U "_"

const Order::value_type Scan::scan_i_negatively_j_negatively             = S IN U JN;
const Order::value_type Scan::scan_i_negatively_j_negatively_alternating = S IN U JN AL;
const Order::value_type Scan::scan_i_negatively_j_positively             = S IN U JP;
const Order::value_type Scan::scan_i_negatively_j_positively_alternating = S IN U JP AL;
const Order::value_type Scan::scan_i_positively_j_negatively             = S IP U JN;
const Order::value_type Scan::scan_i_positively_j_negatively_alternating = S IP U JN AL;
const Order::value_type Scan::scan_i_positively_j_positively             = S IP U JP;
const Order::value_type Scan::scan_i_positively_j_positively_alternating = S IP U JP AL;
const Order::value_type Scan::scan_j_negatively_i_negatively             = S JN U IN;
const Order::value_type Scan::scan_j_negatively_i_negatively_alternating = S JN U IN AL;
const Order::value_type Scan::scan_j_negatively_i_positively             = S JN U IP;
const Order::value_type Scan::scan_j_negatively_i_positively_alternating = S JN U IP AL;
const Order::value_type Scan::scan_j_positively_i_negatively             = S JP U IN;
const Order::value_type Scan::scan_j_positively_i_negatively_alternating = S JP U IN AL;
const Order::value_type Scan::scan_j_positively_i_positively             = S JP U IP;
const Order::value_type Scan::scan_j_positively_i_positively_alternating = S JP U IP AL;

#undef IP
#undef IN
#undef JP
#undef JN
#undef AL
#undef S
#undef U


Scan::Scan() :
    Order(scan_i_negatively_j_negatively,              //
          scan_i_negatively_j_negatively_alternating,  //
          scan_i_negatively_j_positively,              //
          scan_i_negatively_j_positively_alternating,  //
          scan_i_positively_j_negatively,              //
          scan_i_positively_j_negatively_alternating,  //
          scan_i_positively_j_positively,              //
          scan_i_positively_j_positively_alternating,  //
          scan_j_negatively_i_negatively,              //
          scan_j_negatively_i_negatively_alternating,  //
          scan_j_negatively_i_positively,              //
          scan_j_negatively_i_positively_alternating,  //
          scan_j_positively_i_negatively,              //
          scan_j_positively_i_negatively_alternating,  //
          scan_j_positively_i_positively,              //
          scan_j_positively_i_positively_alternating) {}

bool Scan::is_scan_i_positive(const value_type& o) const {
    return o == scan_i_positively_j_negatively || o == scan_i_positively_j_positively ||
           o == scan_j_negatively_i_positively || o == scan_j_positively_i_positively ||
           o == scan_i_positively_j_negatively_alternating || o == scan_i_positively_j_positively_alternating ||
           o == scan_j_negatively_i_positively_alternating || o == scan_j_positively_i_positively_alternating;
}


bool Scan::is_scan_j_positive(const value_type& o) const {
    return o == scan_i_positively_j_positively || o == scan_i_negatively_j_positively ||
           o == scan_j_positively_i_positively || o == scan_j_positively_i_negatively ||
           o == scan_i_positively_j_positively_alternating || o == scan_i_negatively_j_positively_alternating ||
           o == scan_j_positively_i_positively_alternating || o == scan_j_positively_i_negatively_alternating;
}


bool Scan::is_scan_ij(const value_type& o) const {
    return o == scan_i_positively_j_negatively || o == scan_i_negatively_j_negatively ||
           o == scan_i_positively_j_positively || o == scan_i_negatively_j_positively ||
           o == scan_i_positively_j_negatively_alternating || o == scan_i_negatively_j_negatively_alternating ||
           o == scan_i_positively_j_positively_alternating || o == scan_i_negatively_j_positively_alternating;
}


bool Scan::is_scan_alternating_direction(const value_type& o) const {
    return !(o == scan_i_positively_j_negatively || o == scan_i_negatively_j_negatively ||
             o == scan_i_positively_j_positively || o == scan_i_negatively_j_positively ||
             o == scan_j_negatively_i_positively || o == scan_j_negatively_i_negatively ||
             o == scan_j_positively_i_positively || o == scan_j_positively_i_negatively);
}


}  // namespace eckit::geo::order
