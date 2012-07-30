/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_CallbackLogger_H
#define eclib_CallbackLogger_H

#include "eclib/machine.h"

#include "eclib/Logger.h"

class CallbackBuffer;

class CallbackLogger : public Logger {
public:
    
    // typedefs
    
    typedef void (*callback) (void* ctxt, int level, const char* msg);

    // -- Constructor

    CallbackLogger();

    // -- Destructor

    virtual ~CallbackLogger();
    
    // -- Methods
    
    void register_callback(callback c, int level, void* ctxt);
    
    virtual void beginLine();
    
    virtual void endLine();
    
    virtual std::ostream& out();

protected:
  
  std::streambuf::int_type overflow (std::streambuf::int_type);
  
  std::streambuf::int_type sync();
  
  bool  dumpBuffer();
    
private:
    
    CallbackBuffer* buffer_;
    std::ostream*   out_;
  
};

#endif // eclib_CallbackLogger_h
