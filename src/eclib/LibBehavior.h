/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_LibBehavior_H
#define eclib_LibBehavior_H

#include "eclib/CallbackLogger.h"
#include "eclib/StandardBehavior.h"

class LibBehavior : public StandardBehavior {
public:

    // -- Contructors

	LibBehavior();

    // -- Destructor

	~LibBehavior();
        
    void register_logger_callback(CallbackLogger::callback c, void* ctxt = 0 );
    
public: // interface methods
        
    virtual std::string home() const;    
    
    virtual Logger* createInfoLogger();
    virtual Logger* createDebugLogger();
    virtual Logger* createWarningLogger();
    virtual Logger* createErrorLogger();
    
protected:
    
    CallbackLogger* info_;
    CallbackLogger* debug_;
    CallbackLogger* warn_;
    CallbackLogger* error_;
};

#endif // eclib_LibBehavior_h
