// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosObject.h"

#include <cstddef>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/PartHandle.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#include "eckit/io/rados/RadosMultiObject.h"
#include "eckit/io/rados/RadosName.h"
#include "eckit/io/rados/RadosNamespace.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosObject::RadosObject(const eckit::URI& uri) :
    RadosObject(rados::namesIn(uri, 3, "rados:<pool>/<namespace>/<name>")) {}

RadosObject::RadosObject(const std::vector<std::string>& names) : RadosObject(names[0], names[1], names[2]) {}

RadosObject::RadosObject(const std::string& pool, const std::string& nspace, const std::string& oid) :
    ns_(pool, nspace), oid_(oid) {
    rados::checkName("object", oid_);
}

eckit::URI RadosObject::uri() const {
    return rados::uriOf(str(), ns_.name());
}

std::string RadosObject::str() const {
    return ns_.pool().name() + '/' + ns_.name() + '/' + oid_;
}

bool RadosObject::exists() const {

    return eckit::RadosCluster::instance().exists(*this);
}

bool RadosObject::isPart() const {
    return eckit::RadosMultiObject::isPart(*this);
}

void RadosObject::ensureDestroyed() const {
    eckit::RadosCluster::instance().remove(*this);
}

void RadosObject::ensureAllDestroyed() const {
    eckit::RadosMultiObject(*this).remove();
}

eckit::DataHandle* RadosObject::dataHandle(RadosWriteMode mode, size_t maxPendingBytes) const {
    if (mode == RadosWriteMode::Async) {
        return new eckit::RadosAsyncHandle(*this, maxPendingBytes);
    }
    return new eckit::RadosHandle(*this);
}

eckit::DataHandle* RadosObject::rangeReadHandle(const eckit::Offset& off, const eckit::Length& len) const {
    return new eckit::PartHandle(dataHandle(), off, len);
}

eckit::DataHandle* RadosObject::multipartWriteHandle(const eckit::Length& maxPartSize, RadosWriteMode mode,
                                                     size_t maxPendingBytes, size_t maxOpenParts) const {
    return new eckit::RadosMultiObjWriteHandle(*this, maxPartSize, mode, maxPendingBytes, maxOpenParts);
}

eckit::DataHandle* RadosObject::multipartReadHandle() const {
    return new eckit::RadosMultiObjReadHandle(*this);
}

eckit::DataHandle* RadosObject::multipartRangeReadHandle(const eckit::Offset& off, const eckit::Length& len) const {
    return new eckit::PartHandle(multipartReadHandle(), off, len);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
