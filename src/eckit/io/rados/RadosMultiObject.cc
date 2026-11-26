// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosMultiObject.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <optional>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosAttributes.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

const std::string partSuffix = ";part-";

const char* const lengthAttr      = "length";
const char* const partsAttr       = "parts";
const char* const maxPartSizeAttr = "maxsize";

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

RadosMultiObject::RadosMultiObject(const RadosObject& object) : object_(object) {}

RadosObject RadosMultiObject::part(size_t index) const {

    if (index == 0) {
        return object_;
    }

    return {object_.nspace().pool().name(), object_.nspace().name(),
            object_.name() + partSuffix + std::to_string(index)};
}

bool RadosMultiObject::isPart(const RadosObject& object) {

    const std::string& name = object.name();

    const auto pos = name.rfind(partSuffix);
    if (pos == std::string::npos || pos == 0) {
        return false;
    }

    const auto index = name.begin() + pos + partSuffix.size();

    return index != name.end() && std::all_of(index, name.end(), [](unsigned char c) { return std::isdigit(c) != 0; });
}

std::optional<RadosMultiObject::Layout> RadosMultiObject::layout() const {

    const auto attr = RadosCluster::instance().attributes(object_);
    if (!attr) {
        return std::nullopt;
    }

    Layout layout;
    if (!attr->get(lengthAttr, layout.length) || !attr->get(partsAttr, layout.parts) ||
        !attr->get(maxPartSizeAttr, layout.maxPartSize)) {
        return std::nullopt;
    }

    return layout;
}

void RadosMultiObject::layout(const Layout& layout) const {

    RadosAttributes attr;
    attr.set(lengthAttr, layout.length);
    attr.set(partsAttr, layout.parts);
    attr.set(maxPartSizeAttr, layout.maxPartSize);

    RadosCluster::instance().attributes(object_, attr);
}

bool RadosMultiObject::remove() const {

    RadosCluster& cluster = RadosCluster::instance();

    const auto attr = cluster.attributes(object_);
    if (!attr) {
        return false;
    }

    /// @note: an object which was not written as a multipart object has no such attribute
    size_t parts = 1;
    attr->get(partsAttr, parts);

    /// @note: the first part holds the attributes which describe the others, so it goes last.
    ///   A removal which failed half way can then be repeated.
    for (size_t i = parts; i-- > 1;) {
        cluster.remove(part(i));
    }

    return cluster.remove(object_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
