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
#include <vector>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid {


class FESOM final : public Unstructured {
public:
    // -- Constructors

    explicit FESOM(const Spec&);
    explicit FESOM(uid_t);

    // -- Overridden methods

    uid_t calculate_uid() const override;

private:
    // -- Members

    uid_t uid_;
    Arrangement arrangement_;
    std::string path_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
