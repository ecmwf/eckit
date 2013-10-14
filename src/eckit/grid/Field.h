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

#ifndef eckit_grid_Field_H
#define eckit_grid_Field_H

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {

//-----------------------------------------------------------------------------

class Field : private eckit::NonCopyable {

public: // methods

    Field();
    
    virtual ~Field();

protected:

};

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

#endif
