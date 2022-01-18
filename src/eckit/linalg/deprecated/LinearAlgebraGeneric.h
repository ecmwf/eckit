/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/dense/LinearAlgebraGeneric.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace deprecated {


class LinearAlgebraGeneric final : public LinearAlgebra {
public:
    LinearAlgebraGeneric(const std::string& name) :
        LinearAlgebra(name) {}

private:
    const dense::LinearAlgebraGeneric lad_;
    const sparse::LinearAlgebraGeneric las_;

    const LinearAlgebraDense& laDense() const override { return lad_; }
    const LinearAlgebraSparse& laSparse() const override { return las_; }

    void print(std::ostream&) const override;
};


}  // namespace deprecated
}  // namespace linalg
}  // namespace eckit
