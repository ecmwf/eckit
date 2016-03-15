/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/log/Colour.h"
#include "eckit/log/FormatBuffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ColorizeFormat : public FormatBuffer {
public:
    
    typedef std::ostream& (*oproc)(std::ostream&);
    
    ColorizeFormat( std::size_t size = 1024 );
    
    virtual ~ColorizeFormat(){ pubsync(); }
    
    virtual void beginLine();
    virtual void endLine();

    void setColor( oproc c )   { begin_ = c; }
    void resetColor( oproc c ) { end_   = c; }
        
    std::string prefix() const;
    void prefix(const std::string &);
    
protected:
    
    std::string   prefix_;
    oproc         begin_;
    oproc         end_;    
    
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
