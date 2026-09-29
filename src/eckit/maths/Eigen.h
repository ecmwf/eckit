// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file Eigen.h
/// @author Tiago Quintino
/// @date March 2014

#ifndef eckit_maths_Eigen_h
#define eckit_maths_Eigen_h

#include "eckit/eckit.h"

//--------------------------------------------------------------------------------------------

#if eckit_HAVE_EIGEN

#define EIGEN_NO_AUTOMATIC_RESIZING
#define EIGEN_DONT_ALIGN
#define EIGEN_DONT_VECTORIZE

#ifdef ECKIT_CONTRIB_EIGEN

#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/Dense"
#include "eigen3/Eigen/Geometry"
#include "eigen3/Eigen/Sparse"

#else

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#endif

#endif

//--------------------------------------------------------------------------------------------

#endif
