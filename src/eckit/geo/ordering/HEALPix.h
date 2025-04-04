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

#include "eckit/geo/Ordering.h"


namespace eckit::geo::ordering {


class HEALPix final : public Ordering {
public:

    // -- Methods

    int size() const { return 12 * Nside_ * Nside_; }
    int nside() const { return Nside_; }

    int ring_to_nest(int r) const;
    int nest_to_ring(int r) const;

    // -- Overriden methods

    const std::string& type() const override;
    void fill_spec(spec::Custom&) const override;

    Ordering::ordering_type to_type() const override { return from_type_; }
    Ordering::ordering_type from_type() const override { return to_type_; }

    Reorder reorder() const override;

protected:

    // -- Constructors

    explicit HEALPix(int Nside, Ordering::ordering_type from, Ordering::ordering_type to);

private:

    // -- Members

    const Ordering::ordering_type from_type_;
    const Ordering::ordering_type to_type_;

    const int Nside_;  // up to 2^13
    const int Npix_;
    const int Ncap_;
    const int k_;
};


}  // namespace eckit::geo::ordering
