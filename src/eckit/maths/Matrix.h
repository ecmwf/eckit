/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
