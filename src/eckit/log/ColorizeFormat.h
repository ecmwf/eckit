/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ColorizeFormat.h
/// @author Tiago Quintino

#ifndef eckit_log_ColorizeFormat_h
#define eckit_log_ColorizeFormat_h

#include "eckit/log/FormatBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Capitalizer : public FormatBuffer {
public:
    Capitalizer( std::size_t size = 1024 ) : FormatBuffer(size) {}
    virtual ~Capitalizer(){ pubsync(); }
    virtual void beginLine() { *target() << " << "; }
    virtual void endLine()   { *target() << " >> "; }    
    virtual void process(const char *begin, const char *end)
    {
        const char* p = begin;
        while (p != end) {
            *target() << std::toupper(*p,loc_);
            ++p;
        }
    }
};

//-----------------------------------------------------------------------------

class ColorizeFormat : public FormatBuffer {
public:
    
    typedef std::ostream& (*oproc)(std::ostream&);
    
    ColorizeFormat( std::size_t size = 1024 );
    
    virtual ~ColorizeFormat(){ pubsync(); }
    
    virtual void beginLine() {}
    virtual void endLine()   {}    
    virtual void process(const char *begin, const char *end);

    void setColor( oproc c )   { begin_ = c; }
    void resetColor( oproc c ) { end_   = c; }
        
protected:
    
    oproc         begin_;
    oproc         end_;    
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
