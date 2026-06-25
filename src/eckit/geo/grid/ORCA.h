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

#include <cstddef>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/Grid.h"


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

    size_t nx() const { return shape().at(1); }
    size_t ny() const { return shape().at(0); }

    std::string name() const { return name_; }
    std::string arrangement() const;

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    uid_type calculate_uid() const override;

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    void cache() const override;

    BoundingBox* calculate_bbox() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;
    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    const order_type& order() const override;
    renumber_type reorder(const order_type& to) const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec_from_uid(const std::string& name);

    [[nodiscard]] static Arrangement arrangement_from_string(const std::string&);
    [[nodiscard]] static std::string arrangement_to_string(Arrangement);

private:

    // -- Methods

    [[nodiscard]] const cache::LatitudeLongitude& record() const;

    // -- Members

    const std::string name_;
    const Arrangement arrangement_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
