/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   types.h
/// @author Florian Rathgeber
/// @author Tiago Quintino
/// @date   June 2015

#ifndef eckit_la_types_h
#define eckit_la_types_h

#include <cstddef>

namespace eckit {
namespace linalg {

typedef double  Scalar;
typedef int     Index;
typedef size_t  Size;

class Vector;
class Matrix;
class SparseMatrix;

}  // namespace linalg
} // namespace eckit

#endif
