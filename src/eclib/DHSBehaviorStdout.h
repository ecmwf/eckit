/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_DHSBehaviorStdout_h
#define eclib_DHSBehaviorStdout_h

#include "eclib/machine.h"
#include "eclib/DHSBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class DHSBehaviorStdout : public DHSBehavior {
public:

    // -- Contructors

    DHSBehaviorStdout(ostream& out);

    // -- Destructor

    ~DHSBehaviorStdout();
    
public:
    virtual DHSLogger* getLogger();
    ostream& out_;
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif // eclib_DHSBehaviorStdout_h
