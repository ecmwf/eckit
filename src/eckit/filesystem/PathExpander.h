/*
 * (C) Copyright 1996- ECMWF.
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

#include <iosfwd>
#include <map>
#include <string>

#include "eckit/memory/NonCopyable.h"
#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PathExpander : private eckit::NonCopyable {
public:  // methods

    static std::string expand(const std::string& path);

protected:  // methods

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const = 0;

    PathExpander(const std::string&);

    virtual ~PathExpander();

    virtual void print(std::ostream&) const;

private:  // members

    std::string name_;

    friend std::ostream& operator<<(std::ostream& s, const PathExpander& p);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
