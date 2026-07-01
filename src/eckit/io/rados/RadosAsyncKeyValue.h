/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
/// @date June 2024

#pragma once

#include "eckit/io/rados/RadosKeyValue.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosAsyncKeyValue : public RadosKeyValue {

public: // methods

    RadosAsyncKeyValue(const eckit::URI&, size_t maxAioBuffSize = 1024 * 1024);
    RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid, size_t maxAioBuffSize = 1024 * 1024);

    void ensureCreated();
    /// @todo:
    // void ensureDestroyed();

    long put(const std::string& key, const void*, const long&);
    void remove(const std::string& key);

    void flush();

private: // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> comps_;

    size_t maxAioBuffSize_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
