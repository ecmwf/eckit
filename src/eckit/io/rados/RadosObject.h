// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date June 2019

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosNamespace.h"

namespace eckit {

class DataHandle;

/// Whether a write returns once it is done, or is only submitted, to be waited for on flush or close
enum class RadosWriteMode {
    Sync,
    Async
};

constexpr size_t radosDefaultMaxPendingBytes = 1024 * 1024;
constexpr size_t radosDefaultMaxOpenParts    = 1024;

//----------------------------------------------------------------------------------------------------------------------

class RadosObject {
public:

    explicit RadosObject(const eckit::URI& uri);

    RadosObject(const std::string& pool, const std::string& nspace, const std::string& oid);

    const eckit::RadosNamespace& nspace() const { return ns_; }
    const std::string& name() const { return oid_; }
    /// @throws UserError for an object in the default namespace, which a URI cannot express
    eckit::URI uri() const;
    std::string str() const;

    bool exists() const;

    /// @returns whether this is a part, other than the first, of a multipart object. See RadosMultiObject.
    bool isPart() const;

    /// Removes the object, if it exists
    void ensureDestroyed() const;
    /// Removes the object and, if it was written as a multipart object, all its parts
    void ensureAllDestroyed() const;

    // -- handles, owned by the caller

    /// Reads or writes the object, which must fit in one object of the cluster
    /// @param maxPendingBytes for asynchronous writes: amount of data not yet waited for, above which a write flushes
    eckit::DataHandle* dataHandle(RadosWriteMode mode    = RadosWriteMode::Sync,
                                  size_t maxPendingBytes = radosDefaultMaxPendingBytes) const;

    /// Reads a range of the object
    eckit::DataHandle* rangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;

    /// Writes data of any size, as a multipart object. See RadosMultiObject.
    /// @param maxPartSize size of the parts; if 0, the largest object the cluster accepts
    /// @param maxOpenParts for asynchronous writes: number of parts written at a time
    eckit::DataHandle* multipartWriteHandle(const eckit::Length& maxPartSize = 0,
                                            RadosWriteMode mode              = RadosWriteMode::Sync,
                                            size_t maxPendingBytes           = radosDefaultMaxPendingBytes,
                                            size_t maxOpenParts              = radosDefaultMaxOpenParts) const;

    /// Reads a multipart object, or a range of it
    eckit::DataHandle* multipartReadHandle() const;
    eckit::DataHandle* multipartRangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;

private:  // methods

    explicit RadosObject(const std::vector<std::string>& names);

private:  // members

    eckit::RadosNamespace ns_;
    std::string oid_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
