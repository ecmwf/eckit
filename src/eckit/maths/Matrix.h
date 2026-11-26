// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @author Willem Deconinck
 * @date   Sept 2014
 *
 * This file introduces classes
 *  - RowVector
 *  - ColVector
 *  - Matrix
 *
 * By default, they are straight inherited from Eigen classes
 * Due to current inability to compile Eigen on CRAY,
 * Transitional classes have been created, which only implement
 * a small subset of Eigen functionality, and to let at least
 * code be compiled
 * It is strongly advised to only use the subset in the
 * transitional classes, or implement as needed
 */
#ifndef eckit_maths_Matrix_h
#define eckit_maths_Matrix_h

#include "eckit/eckit.h"

namespace eckit::maths {

template <typename scalar, typename index>
class RowVector;
template <typename scalar, typename index>
class ColVector;
template <typename scalar, typename index>
class Matrix;

}  // namespace eckit::maths

#if eckit_HAVE_EIGEN

// Implementation using Eigen
#include "eckit/maths/MatrixEigen.h"

#else

// Own implementation using Lapack if available
#include "eckit/maths/MatrixLapack.h"

#endif

#endif
