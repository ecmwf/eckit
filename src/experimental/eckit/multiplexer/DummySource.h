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

#ifndef eckit_multiplexer_DummySource_H
#define eckit_multiplexer_DummySource_H

#include "eckit/memory/ScopedPtr.h"

#include "eckit/multiplexer/DataSink.h"

namespace eckit {
namespace multiplexer {

// --------------------------------------------------------------------------------------------------

class DummySource : private eckit::NonCopyable {

public:

    DummySource(DataSink& sink, const std::string& sink_open_key);

    ~DummySource();

    void generate_field();

private:

    DataSink& dataSink_;

};

// --------------------------------------------------------------------------------------------------

} // multiplexer
} // eckit

#endif
