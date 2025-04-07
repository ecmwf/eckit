/*
 * (C) Copyright 1996- ECMWF.
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

#include <set>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class StreamParser : private NonCopyable {

public:  // types

    class Error : public Exception {
    public:

        Error(const std::string& what, size_t line = 0);
    };

public:  // methods

    StreamParser(std::istream& in, bool comments = false, const char* comment = "#");
    virtual ~StreamParser() = default;

    char peek(bool spaces = false);
    char next(bool spaces = false);

    void consume(char);
    void consume(const char*);

    void expect(const char*);
    void putback(char);

protected:  // members

    size_t line_;
    size_t pos_;
    bool comments_;

private:  // members

    std::istream& in_;

    std::set<char> comment_;

    char _get();
    char _peek();
    bool _eof();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
