/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015


#ifndef eckit_JSONMetadata_h
#define eckit_JSONMetadata_h

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/types/Metadata.h"
#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Buffer;


class JSONMetadata : public Metadata {

public: // methods

    JSONMetadata(const Buffer& buffer);
    virtual ~JSONMetadata();

    virtual std::vector<std::string> keywords() const;

    virtual bool has(const std::string& name) const;

    virtual void get(const std::string& name, std::string& value) const;
    virtual void get(const std::string& name, long& value) const;
    virtual void get(const std::string& name, double& value) const;

    friend std::ostream& operator<<(std::ostream& s, const JSONMetadata& p) {
        p.print(s);
        return s;
    }

protected: // methods

    virtual void print(std::ostream&) const;

private: // members

    Value root_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif // eckit_JSONMetadata_h
