/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_StandardBehavior_h
#define eckit_StandardBehavior_h

#include "eclib/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StandardBehavior : public ContextBehavior {
public:

    /// Contructors

	StandardBehavior();

    /// Destructor

	~StandardBehavior();
    
public: // methods
    
    virtual string runName() const;
    virtual void runName( const string& name ); 
        
    virtual long taskId() const;
    
    virtual Logger* createInfoLogger();
    virtual Logger* createDebugLogger();
    virtual Logger* createWarningLogger();
    virtual Logger* createErrorLogger();

private: // members
    
    std::string name_;
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_StandardBehavior_h
