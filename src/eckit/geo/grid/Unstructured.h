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
#include <memory>

#include "eckit/geo/Grid.h"
#include "eckit/geo/container/PointsContainer.h"


namespace eckit::geo::iterator {
class Unstructured;
}  // namespace eckit::geo::iterator


namespace eckit::geo::grid {


class Unstructured : public Grid {
public:

    // -- Constructors

    explicit Unstructured(const Spec& spec) : Grid(spec) {}

    explicit Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes);
    explicit Unstructured(const std::vector<Point>&);
    explicit Unstructured(std::vector<Point>&&);

    // -- Methods

    std::shared_ptr<container::PointsContainer> container() const { return container_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;
    std::vector<size_t> shape() const override { return {size()}; }

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    [[nodiscard]] std::vector<Point> to_points() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);
    [[nodiscard]] static Unstructured* make_from_latlon(const std::vector<double>&, const std::vector<double>&);

protected:

    // -- Constructors

    Unstructured(container::PointsContainer*);

    // -- Methods

    void resetContainer(container::PointsContainer* container) { container_.reset(container); }

private:

    // -- Members

    std::shared_ptr<container::PointsContainer> container_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Friends

    friend class geo::iterator::Unstructured;
};


}  // namespace eckit::geo::grid
