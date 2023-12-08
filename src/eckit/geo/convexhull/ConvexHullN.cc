/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/convexhull/ConvexHullN.h"


namespace eckit::geo::convexhull {


static ConvexHullBuilder<ConvexHullN<2>> __convexhulln2("convex-hull-n-2");
static ConvexHullBuilder<ConvexHullN<3>> __convexhulln3("convex-hull-n-3");
static ConvexHullBuilder<ConvexHullN<4>> __convexhulln4("convex-hull-n-4");
static ConvexHullBuilder<ConvexHullN<5>> __convexhulln5("convex-hull-n-5");
static ConvexHullBuilder<ConvexHullN<6>> __convexhulln6("convex-hull-n-6");
static ConvexHullBuilder<ConvexHullN<7>> __convexhulln7("convex-hull-n-7");
static ConvexHullBuilder<ConvexHullN<8>> __convexhulln8("convex-hull-n-8");
static ConvexHullBuilder<ConvexHullN<9>> __convexhulln9("convex-hull-n-9");
// ...


}  // namespace eckit::geo::convexhull
