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

#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid::unstructured {


class UnstructuredGeneric final : public Unstructured {
public:

    // -- Constructors

    using Unstructured::Unstructured;

    // -- Methods

    const std::string& name() const { return name_; }
    const std::string& name(const std::string& another) { return name_ = another; }

    // -- Overridden methods

    uid_t calculate_uid() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

private:

    // -- Members

    std::string name_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid::unstructured
