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

#include <array>
#include <cstddef>
#include <cstdint>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/container/PointsContainer.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid {


class ORCA final : public Regular {
public:

    // -- Types

    struct ORCARecord {
        explicit ORCARecord() = default;

        void read(const PathName&);
        void check(const Spec&) const;
        size_t write(const PathName&, const std::string& compression = "none");
        uid_t calculate_uid(Arrangement) const;

        using bytes_t = decltype(sizeof(int));
        bytes_t footprint() const;

        size_t ni() const;
        size_t nj() const;

        std::array<std::int32_t, 2> dimensions_ = {-1, -1};
        std::array<std::int32_t, 4> halo_       = {-1, -1, -1, -1};
        std::array<double, 2> pivot_            = {-1, -1};

        std::vector<double> longitudes_;
        std::vector<double> latitudes_;
        std::vector<std::byte> flags_;
    };

    // -- Constructors

    explicit ORCA(const Spec&);
    explicit ORCA(uid_t);

    ORCA(const std::string& name, Arrangement);

    // -- Methods

    size_t nx() const override { return record_.nj(); }
    size_t ny() const override { return record_.ni(); }

    std::string name() const { return name_; }
    std::string arrangement() const;

    std::shared_ptr<container::PointsContainer> container() const { return container_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    uid_t calculate_uid() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }  // FIXME: not sure this is semanticaly correct
    bool isPeriodicWestEast() const override { return true; }

    std::vector<Point> to_points() const override;
    std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    const order_type& order() const override;
    Reordering reorder(const order_type& to) const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    [[nodiscard]] static Arrangement arrangement_from_string(const std::string&);
    [[nodiscard]] static std::string arrangement_to_string(Arrangement);

private:

    // -- Members

    std::string name_;
    Arrangement arrangement_;
    const ORCARecord& record_;
    std::shared_ptr<container::PointsContainer> container_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid
