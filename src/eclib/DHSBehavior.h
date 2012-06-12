/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_DHSBehavior_H
#define eclib_DHSBehavior_H

#include "eclib/ContextBehavior.h"

class DHSBehavior : public ContextBehavior {
public:

    // -- Contructors

    DHSBehavior();

    // -- Destructor

    ~DHSBehavior();
    
public: // interface methods
    
    virtual std::string home() const;
    virtual long taskId() const;
    
    virtual Logger* createInfoLogger();
    virtual Logger* createDebugLogger();
    virtual Logger* createWarningLogger();
    virtual Logger* createErrorLogger();

protected:
    
    bool toFile();
    
private: // members
    
    
};

#endif // eclib_DHSBehavior_h
