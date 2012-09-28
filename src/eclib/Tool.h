/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Sep 2012

#ifndef eclib_Tool_H
#define eclib_Tool_H

#include "eclib/Configurable.h"

class Tool : public Configurable {

public: // methods

    /// Contructors

	Tool(int argc,char **argv);

    /// Destructor

	~Tool();
	
    void start();
    
	virtual void run() = 0;
    
protected: // methods

	virtual void   reconfigure();
	virtual std::string name() const   { return name_; }

private: // members

    std::string name_;

};

#endif
