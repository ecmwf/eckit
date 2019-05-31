/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
///
/// @date 21 Mar 2019

#ifndef eckit_deprecated_h
#define eckit_deprecated_h

#if !defined(DEPRECATED) && !defined(ECKIT_NO_DEPRECATION_WARNINGS)

#if (__cplusplus >= 201402L)
#define DEPRECATED(x) [[deprecated]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED(x) __attribute__((deprecated))
#else
#pragma message("WARNING: compiler does not support deprecation of class/functions")
#define DEPRECATED(x) /* deprecated */
#endif
#endif
#else
#define DEPRECATED(x) /* deprecated */
#endif

#endif /* eckit_deprecated_h */
