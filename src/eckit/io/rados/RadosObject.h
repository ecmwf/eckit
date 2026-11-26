// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
    /// @note: the default (empty) namespace cannot be expressed in a URI, so this throws for it
    eckit::URI uri() const;
    std::string str() const;

    bool exists() const;
    void ensureDestroyed() const;
    void ensureAllDestroyed() const;

    eckit::DataHandle* dataHandle() const;
    eckit::DataHandle* asyncDataHandle(size_t maxAioBuffSize = 1024 * 1024) const;
    eckit::DataHandle* rangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;
    eckit::DataHandle* multipartWriteHandle(const eckit::Length& maxPartSize = 0) const;
    eckit::DataHandle* asyncMultipartWriteHandle(const eckit::Length& maxPartSize = 0,
                                                 size_t maxAioBuffSize            = 1024 * 1024,
                                                 size_t maxHandleBuffSize         = 1024) const;
    eckit::DataHandle* multipartRangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;

private:

    eckit::RadosNamespace ns_;
    std::string oid_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
