/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015


#ifndef eckit_Metadata_H
#define eckit_Metadata_H

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Metadata : private eckit::NonCopyable {

public: // methods

    virtual ~Metadata();

    virtual std::vector<std::string> parameters() const = 0;

    virtual bool has(const std::string& name) const = 0;

    virtual void get(const std::string& name, std::string& value) const = 0;
    virtual void get(const std::string& name, long& value) const = 0;
    virtual void get(const std::string& name, double& value) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Metadata& p) {
        p.print(s);
        return s;
    }

protected: // methods

    virtual void print(std::ostream&) const = 0;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

