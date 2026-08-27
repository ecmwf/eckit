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
/// @author Nicolau Manubens
/// @date June 2019

#pragma once

#include <cstddef>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosNamespace.h"

namespace eckit {

class DataHandle;

//----------------------------------------------------------------------------------------------------------------------

class RadosObject {
public:

    explicit RadosObject(const eckit::URI& uri);

    RadosObject(const std::string& pool, const std::string& nspace, const std::string& oid);

    RadosObject(const RadosObject& other, size_t part);

    const eckit::RadosNamespace& nspace() const { return ns_; }
    const std::string& name() const { return oid_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const;

    bool exists() const;
    void ensureDestroyed() const;
    void ensureAllDestroyed() const;

    eckit::DataHandle* dataHandle() const;
    eckit::DataHandle* asyncDataHandle(size_t maxAioBuffSize = 1024 * 1024) const;
    eckit::DataHandle* rangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;
    eckit::DataHandle* multipartWriteHandle(const eckit::Length& maxPartSize = 0) const;
    eckit::DataHandle* asyncMultipartWriteHandle(const eckit::Length& maxPartSize = 0, size_t maxAioBuffSize = 1024,
                                                 size_t maxHandleBuffSize = 1024) const;
    eckit::DataHandle* multipartRangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;

private:

    eckit::RadosNamespace ns_;
    std::string oid_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
