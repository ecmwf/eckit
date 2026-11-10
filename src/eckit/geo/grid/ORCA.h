// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"


namespace eckit::geo::cache {
class LatitudeLongitude;
}


namespace eckit::geo::grid {


class ORCA final : public Grid {
public:

    // -- Constructors

    explicit ORCA(const Spec&);
    explicit ORCA(uid_type);

    ORCA(const std::string& name, Arrangement);

    // -- Methods

    size_t nx() const override { return shape().at(1); }
    size_t ny() const override { return shape().at(0); }

    std::string name() const override { return name_; }
    std::string arrangement() const override;

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const override;

    uid_type calculate_uid() const override;

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    void cache() const override;

    BoundingBox* calculate_bbox() const override;

    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    const order_type& order() const override;
    renumber_type reorder(const order_type& to) const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec_from_uid(const std::string& name);

private:

    // -- Methods

    [[nodiscard]] const cache::LatitudeLongitude& record() const;

    // -- Overridden methods

    std::unique_ptr<Iterator::State> make_state() const override;

    // -- Members

    const std::string name_;
    const Arrangement arrangement_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
