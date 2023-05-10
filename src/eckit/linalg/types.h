/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Florian Rathgeber
/// @author Pedro Maciel
/// @author Tiago Quintino


#pragma once

#include <cstddef>

namespace eckit::linalg {

using Scalar = double;
using Index  = int;
using Size   = size_t;

class Vector;
class Matrix;
class SparseMatrix;

}  // namespace eckit::linalg
