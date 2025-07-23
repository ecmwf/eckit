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

#include "eckit/geo/Grid.h"


namespace eckit::geo::grid {


class SphericalHarmonics final : public Grid {
public:

    // -- Constructors

    explicit SphericalHarmonics(const Spec&);
    explicit SphericalHarmonics(size_t T);

    // -- Methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    size_t truncation() const { return truncation_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    bool empty() const override;
    size_t size() const override { return number_of_complex_coefficients(truncation_); }

    [[nodiscard]] uid_t calculate_uid() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    [[nodiscard]] area::BoundingBox* calculate_bbox() const override;

    void fill_spec(spec::Custom&) const override;

    // -- Class methods

    static size_t number_of_complex_coefficients(size_t truncation);

private:

    // -- Members

    size_t truncation_;
};


}  // namespace eckit::geo::grid
