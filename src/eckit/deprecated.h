// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
