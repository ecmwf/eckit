// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Trace.h"

#include "eckit/config/Resource.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/log/ResourceUsage.h"


namespace eckit::geo {


Trace::Trace(const std::string& name) : Timer(name, Log::debug()) {}


TraceResourceUsage::TraceResourceUsage(const std::string& name) : Trace(name) {
    static bool usage = LibResource<bool, LibEcKitGeo>(
        "eckit-geo-trace-resource-usage;"
        "$ECKIT_GEO_TRACE_RESOURCE_USAGE",
        false);
    info_ = usage ? new eckit::ResourceUsage(name, Log::debug()) : nullptr;
}


TraceResourceUsage::~TraceResourceUsage() {
    delete info_;
}


}  // namespace eckit::geo
