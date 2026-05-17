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

#include <string>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::cache {
class LatitudeLongitude;
}


namespace eckit::geo::grid::unstructured {


class FESOM final : public Unstructured {
public:

    // -- Constructors

    explicit FESOM(const Spec&);
    explicit FESOM(uid_type);

    FESOM(const std::string& name, Arrangement);

    // -- Methods

    std::string name() const { return name_; }
    std::string arrangement() const;

    // -- Overridden methods

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    uid_type calculate_uid() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    [[nodiscard]] static Arrangement arrangement_from_string(const std::string&);
    [[nodiscard]] static std::string arrangement_to_string(Arrangement);

private:

    // -- Constructors

    FESOM(const uid_type&, const std::string& arrangement, const std::string& name);

    // -- Members

    const std::string name_;
    const Arrangement arrangement_;

    // -- Overridden methods

    const std::vector<double>& longitudes() const override;
    const std::vector<double>& latitudes() const override;

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::unstructured
