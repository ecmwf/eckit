/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Tensor.h"

namespace eckit {
namespace linalg {

// Explicit template instantiation to minimise dynamic library code bloat
template class Tensor<double>;
template class Tensor<float>;

}  // namespace linalg
}  // namespace eckit
