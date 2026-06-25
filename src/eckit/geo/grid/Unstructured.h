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


namespace eckit::geo {
namespace cache {
class LatitudeLongitude;
}
namespace iterator {
class Unstructured;
}
}  // namespace eckit::geo


namespace eckit::geo::grid {


class Unstructured : public Grid {
public:

    // -- Constructors

    explicit Unstructured(const Spec&);
    explicit Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes,
                          const std::string& name = "");

    using Grid::Grid;

    // -- Methods

    const std::string& name() const { return name_; }
    const std::string& arrangement() const { return arrangement_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    std::vector<size_t> shape() const override;

    BoundingBox* calculate_bbox() const override;

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    uid_type calculate_uid() const override;
    const std::string& type() const override;
    void cache() const override;

    // -- Class methods

    [[nodiscard]] static uid_type uid_from_latlons(const std::vector<double>&, const std::vector<double>&);

protected:

    // -- Constructors

    explicit Unstructured(const uid_type&, const std::string& name = "");
    explicit Unstructured(const uid_type&, const std::string& name, const std::string& arrangement);

    // -- Methods

    [[nodiscard]] const cache::LatitudeLongitude& record() const;

private:

    // -- Members

    std::string name_;
    std::string arrangement_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Friends

    friend class geo::iterator::Unstructured;
};


}  // namespace eckit::geo::grid
