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

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid::unstructured {


class ICON final : public Unstructured {
public:

    // -- Constructors

    explicit ICON(const Spec&);
    explicit ICON(const uid_type&);
    ICON(const std::string& name, Arrangement);

    // -- Overridden methods

    const std::string& type() const override;
    std::vector<size_t> shape() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string&);
};


}  // namespace eckit::geo::grid::unstructured
