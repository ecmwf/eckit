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
#include "eckit/linalg/dense/LinearAlgebraViennaCL.h"
#include "eckit/linalg/sparse/LinearAlgebraViennaCL.h"


namespace eckit {
namespace linalg {
namespace deprecated {


class LinearAlgebraViennaCL final : public LinearAlgebra {
public:
    LinearAlgebraViennaCL(const std::string& name) :
        LinearAlgebra(name) {}

private:
    const dense::LinearAlgebraViennaCL lad_;
    const sparse::LinearAlgebraViennaCL las_;

    const LinearAlgebraDense& laDense() const override { return lad_; }
    const LinearAlgebraSparse& laSparse() const override { return las_; }

    void print(std::ostream&) const override;
};


}  // namespace deprecated
}  // namespace linalg
}  // namespace eckit
