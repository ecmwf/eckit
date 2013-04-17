/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Sep 2012

#ifndef eckit_StreamParser_h
#define eckit_StreamParser_h

#include "eckit/machine.h"

#include "eckit/memory/NonCopyable.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StreamParser : private NonCopyable  {

public: // types
    
    class Error : public exception {
        string what_;
        virtual const char* what() const  throw() { return what_.c_str(); }
    public:
        Error(const string& what);
        virtual ~Error() throw() {}
    };
    
public: // methods
    
    StreamParser(std::istream& in);

    char peek(bool spaces = false);
    char next(bool spaces = false);

    void consume(char);
    void consume(const char*);

    void expect(const char*);

private: // members

    std::istream& in_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
