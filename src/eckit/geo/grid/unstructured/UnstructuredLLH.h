// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Grid.h"


namespace eckit::geo::grid::unstructured {


// (longitude, latitude, height) points, heights along the normal of the figure (negative: depths below it). The
// coordinates are kept in process
// memory by uid while in use (so a spec of the uid builds the grid again), not in cache files as unstructured_ll
class UnstructuredLLH final : public Grid {
public:

    // -- Types

    using Coordinates = std::array<std::vector<double>, 3>;

    // -- Constructors

    explicit UnstructuredLLH(const Spec&);
    UnstructuredLLH(std::vector<double> longitudes, std::vector<double> latitudes, std::vector<double> heights);

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const override;

    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    std::vector<size_t> shape() const override { return {coordinates_->front().size()}; }

    const std::string& type() const override;

    BoundingBox* calculate_bbox() const override;

    uid_type calculate_uid() const override;

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Types

    using Registered = std::pair<uid_type, std::shared_ptr<const Coordinates>>;

    // -- Constructors

    UnstructuredLLH(Registered&&, std::unique_ptr<const Projection>);

    // -- Members

    std::shared_ptr<const Coordinates> coordinates_;

    // -- Class methods

    static uid_type uid_of(const Coordinates&);
    static Registered regist(Coordinates&&);
    static Registered find(const uid_type&);
};


}  // namespace eckit::geo::grid::unstructured
