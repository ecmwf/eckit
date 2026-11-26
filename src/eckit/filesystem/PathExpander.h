// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   June 2017

#ifndef eckit_PathExpander_h
#define eckit_PathExpander_h

#include <iosfwd>
#include <string>

#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class PathExpander {
public:  // methods

    static std::string expand(const std::string& path);

protected:  // methods

    virtual void expand(const std::string& var, const std::string& path, eckit::StringDict& vars) const = 0;

    PathExpander(const std::string&);

    PathExpander(const PathExpander&)            = delete;
    PathExpander& operator=(const PathExpander&) = delete;
    PathExpander(PathExpander&&)                 = delete;
    PathExpander& operator=(PathExpander&&)      = delete;

    virtual ~PathExpander();

    virtual void print(std::ostream&) const;

private:  // members

    std::string name_;

    friend std::ostream& operator<<(std::ostream& s, const PathExpander& p);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
