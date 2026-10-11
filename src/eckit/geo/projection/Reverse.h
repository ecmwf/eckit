// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Projection with forward and inverse methods swapped
class Reverse final : public Projection {
public:

    // -- Constructors

    /// Takes ownership
    explicit Reverse(const Projection*);

    // -- Overridden methods

    using Projection::fwd;
    using Projection::inv;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    const std::string& type() const override;
    void fill_spec(spec::Custom&) const override;

    bool reverse() const override;

protected:

    // -- Overridden methods

    std::vector<std::vector<double>> fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3) const override;
    std::vector<std::vector<double>> inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3) const override;

private:

    // -- Members

    const std::unique_ptr<const Projection> projection_;
};


}  // namespace eckit::geo::projection
