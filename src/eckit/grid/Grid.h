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

#ifndef eckit_grid_Grid_H
#define eckit_grid_Grid_H

#include <cstddef>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "eckit/memory/NonCopyable.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------

struct Point2D
{
    Point2D( double lat, double lon ) : lat_(lat),lon_(lon) {}
    Point2D( ) : lat_(0.0), lon_(0.0) {}
    double lat_;
    double lon_;
};

//-----------------------------------------------------------------------------

struct BoundBox2D
{
    BoundBox2D( const Point2D& bottom_left, const Point2D& top_right ) :
        bottom_left_(bottom_left),
        top_right_(top_right)
    {
        ASSERT( bottom_left_.lat_ < top_right_.lat_ );
        ASSERT( bottom_left_.lon_ < top_right_.lon_ );
    }

    Point2D bottom_left_;
    Point2D top_right_;
};

//-----------------------------------------------------------------------------

class Grid : private eckit::NonCopyable {

public: // methods

    Grid();

    virtual ~Grid();

    virtual size_t dataSize() const = 0;
    virtual const std::vector<Point2D>& gridData() const = 0;
    virtual BoundBox2D boundingBox() const = 0;

protected:

};

typedef boost::shared_ptr<Grid> GridPtr;

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

#endif
