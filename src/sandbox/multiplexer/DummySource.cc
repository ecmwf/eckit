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
/// @date Dec 2015

#include <iosfwd>
#include "grib_api.h"

#include "sandbox/multiplexer/DataSink.h"
#include "sandbox/multiplexer/DummySource.h"

namespace eckit {
namespace multiplexer {

DummySource::DummySource(const std::string& sink_key)
: dataSink_(DataSinkFactory::build(sink_key)) {

    eckit::Log::info() << "DummySource constructor" << std::endl;
}

DummySource::~DummySource() {
}

} // eckit
} // multiplexer

