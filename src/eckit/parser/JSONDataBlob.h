/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   Jan 2016

#ifndef eckit_JSONDataBlob_h
#define eckit_JSONDataBlob_h

#include "eckit/io/DataBlob.h"
#include "eckit/parser/JSONMetadata.h"

namespace eckit {

// -------------------------------------------------------------------------------------------------

class JSONDataBlob : public eckit::DataBlob {

public: // methods

    JSONDataBlob(const void* data, size_t length);

    virtual ~JSONDataBlob();

    virtual const eckit::Metadata& metadata() const;

private: // members

    virtual void print(std::ostream&) const;

private: // members

    JSONMetadata metadata_;

};

// -------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_JSONDataBlob_h
