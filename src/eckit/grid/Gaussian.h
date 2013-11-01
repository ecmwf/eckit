/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @date Oct 2013

#ifndef eckit_grid_Gaussian_H
#define eckit_grid_Gaussian_H

#include <cstddef>
#include <vector>

#include "eckit/types/Coord.h"
#include "eckit/grid/Grid.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {

//-----------------------------------------------------------------------------

class Gaussian : public Grid {

public: // methods

    Gaussian( size_t resolution, const BoundBox2D& bb );

    virtual ~Gaussian();

    virtual size_t dataSize() const;
    virtual const std::vector<Point2D>& gridData() const { return points_; }
    virtual BoundBox2D boundingBox() const;

protected:

    // Generates latitudes in N hemisphere 
    void generateLatitudes(std::vector<double>& lats);

    size_t resolution_;                 ///< number of longitude increments - can be any size as no requirement for 

    std::vector< Point2D > points_;     ///< storage of coordinate points

    BoundBox2D bound_box_;              ///< bounding box for the domain

private:

    // Generates latitudes in N hemisphere 
    void initialGaussianLatitudes(std::vector<double>& lats);
    
    // Only tested on in N hemisphere 
    void refineLatitude(double& value);
};

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

#endif
