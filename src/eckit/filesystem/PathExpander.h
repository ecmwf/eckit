/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   June 2017

#ifndef eckit_PathExpander_h
#define eckit_PathExpander_h

#include <string>
#include <map>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PathExpander : private eckit::NonCopyable {

public: // methods

    static std::string expand(const std::string& key, const std::string& path);

    virtual std::string expand(const std::string& path) const = 0;

protected: // methods

    PathExpander(const std::string&);

    virtual ~PathExpander();

    virtual void print(std::ostream&) const;

private: // members

    std::string name_;

    friend std::ostream& operator<<(std::ostream& s,const PathExpander& p) { p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
