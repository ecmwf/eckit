/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
