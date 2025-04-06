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

#include "eckit/geo/Order.h"


namespace eckit::geo::order {


class Scan : public Order {
public:

    // -- Methods

    bool is_scan_i_positive(const value_type&) const;
    bool is_scan_j_positive(const value_type&) const;
    bool is_scan_ij(const value_type&) const;
    bool is_scan_alternating_direction(const value_type&) const;

    // -- Class members

    static const value_type scan_i_positively_j_negatively_ij;
    static const value_type scan_i_negatively_j_negatively_ij;
    static const value_type scan_i_positively_j_positively_ij;
    static const value_type scan_i_negatively_j_positively_ij;
    static const value_type scan_i_positively_j_negatively_ji;
    static const value_type scan_i_negatively_j_negatively_ji;
    static const value_type scan_i_positively_j_positively_ji;
    static const value_type scan_i_negatively_j_positively_ji;
    static const value_type scan_i_positively_j_negatively_ij_alternating;
    static const value_type scan_i_negatively_j_negatively_ij_alternating;
    static const value_type scan_i_positively_j_positively_ij_alternating;
    static const value_type scan_i_negatively_j_positively_ij_alternating;
    static const value_type scan_i_positively_j_negatively_ji_alternating;
    static const value_type scan_i_negatively_j_negatively_ji_alternating;
    static const value_type scan_i_positively_j_positively_ji_alternating;
    static const value_type scan_i_negatively_j_positively_ji_alternating;

protected:

    // -- Constructors

    Scan();
};


}  // namespace eckit::geo::order
