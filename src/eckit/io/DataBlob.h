/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jan 2016

#ifndef eckit_DataBlob_h
#define eckit_DataBlob_h

#include "eckit/eckit.h"

#include "eckit/io/Buffer.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"

namespace eckit {

    class Metadata;

//----------------------------------------------------------------------------------------------------------------------

/// Blob of data, thread-safe, reference counted, that can have metadata attached

class DataBlob : public eckit::OwnedLock {

public: // methods

    /// Constructor creates empty buffer
    DataBlob(size_t length);

    /// Constructor copies the data
    DataBlob(const void* data, size_t length);

    virtual ~DataBlob();

    Buffer& buffer();
    const Buffer& buffer() const;
    size_t length() const;

    virtual const eckit::Metadata& metadata() const = 0;

    friend std::ostream& operator<<(std::ostream &s, const DataBlob& p) {
        p.print(s);
        return s;
    }

protected: // methods

    virtual void print(std::ostream&) const = 0;

protected: // members

    eckit::Buffer buffer_;

};

typedef eckit::SharedPtr<eckit::DataBlob> DataBlobPtr;

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
