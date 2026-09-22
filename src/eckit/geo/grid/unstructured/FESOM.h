// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid::unstructured {


class FESOM final : public Unstructured {
public:

    // -- Constructors

    explicit FESOM(const Spec&);
    explicit FESOM(const uid_type&);
    FESOM(const std::string& name, Arrangement);

    // -- Overridden methods

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    uid_type calculate_uid() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string&);
};


}  // namespace eckit::geo::grid::unstructured
