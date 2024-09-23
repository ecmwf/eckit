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

#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid::unstructured {


class FESOM final : public Unstructured {
public:
    // -- Types

    enum Arrangement
    {
        Nodes,
        Centroids,
    };

    // -- Constructors

    explicit FESOM(const Spec&);
    explicit FESOM(uid_t);

private:
    // -- Members

    std::string name_;
    uid_t uid_;
    Arrangement arrangement_;
    std::string path_;

#if 0
    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
#endif
};


}  // namespace eckit::geo::grid::unstructured
