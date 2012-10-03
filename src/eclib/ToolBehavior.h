/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_ToolBehavior_H
#define eclib_ToolBehavior_H

#include "eclib/StandardBehavior.h"

class ToolBehavior : public StandardBehavior {
public:

    // -- Contructors

	ToolBehavior();

    // -- Destructor

	~ToolBehavior();
    
private: // methods
    
    virtual std::string home() const;    
	void setup(int argc, char** argv);
        
};

#endif // eclib_ToolBehavior_h
