// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <string>

#include "eckit/geo/Grid.h"


namespace eckit::geo::grid {


class SphericalHarmonics final : public Grid {
public:

    // -- Constructors

    explicit SphericalHarmonics(const Spec&);
    explicit SphericalHarmonics(size_t T);
    explicit SphericalHarmonics(int T);

    // -- Methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    size_t truncation() const override { return truncation_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    std::string name() const override { return "T" + std::to_string(truncation_); }

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    bool empty() const override;
    size_t size() const override { return number_of_real_coefficients(truncation_); }

    [[nodiscard]] uid_type calculate_uid() const override;

    const Area& area() const override;

    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    void fill_spec(spec::Custom&) const override;

    // -- Class methods

    static size_t number_of_real_coefficients(size_t truncation);
    static size_t number_of_complex_coefficients(size_t truncation);

private:

    // -- Members

    const size_t truncation_;
};


}  // namespace eckit::geo::grid
