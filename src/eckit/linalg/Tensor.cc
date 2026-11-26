// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/Tensor.h"

namespace eckit::linalg {

// Explicit template instantiation to minimise dynamic library code bloat
template class Tensor<double>;
template class Tensor<float>;

}  // namespace eckit::linalg
