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

    eckit::DataHandle* dataHandle() const;
    eckit::DataHandle* asyncDataHandle(size_t maxAioBuffSize = 1024 * 1024) const;
    eckit::DataHandle* rangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;
    eckit::DataHandle* multipartWriteHandle(const eckit::Length& maxPartSize = 0) const;
    eckit::DataHandle* asyncMultipartWriteHandle(const eckit::Length& maxPartSize = 0,
                                                 size_t maxAioBuffSize            = 1024 * 1024,
                                                 size_t maxHandleBuffSize         = 1024) const;
    eckit::DataHandle* multipartRangeReadHandle(const eckit::Offset& offset, const eckit::Length& length) const;

private:  // methods

    explicit RadosObject(const std::vector<std::string>& names);

private:  // members

    eckit::RadosNamespace ns_;
    std::string oid_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
