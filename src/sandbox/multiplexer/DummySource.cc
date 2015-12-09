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

#include "eckit/io/Length.h"
#include "sandbox/multiplexer/DataSink.h"
#include "sandbox/multiplexer/DummySource.h"

namespace eckit {
namespace multiplexer {

//-------------------------------------------------------------------------------------------------

/*
 * SDS: This code shouldn't really be here, but we are trying to do this with raw
 *      grib_api for now, so we don't want to link in the experimental/grib lib
 *      for the meantime.
 *
 * --> Duplicated here for now
 */

#define GRIB_CALL(a) grib_call(a, #a)

int grib_call(int code, const char *msg)
{
	if(code)
	{
        std::ostringstream os;
        os << msg << ": " << grib_get_error_message(code);
        throw Exception(os.str());
	}
	return code;
}

//-------------------------------------------------------------------------------------------------

DummySource::DummySource(DataSink& sink, const std::string& sink_open_key)
: dataSink_(sink) {

    eckit::Log::info() << "DummySource constructor" << std::endl;

    dataSink_.open(sink_open_key);
}

DummySource::~DummySource() {
    dataSink_.close();
}


/*
 * This is a naive routine, that does a little bit too much.
 * It currently generates a scratch grib field, and writes it to the sink.
 */
void DummySource::generate_field() {

    eckit::Log::info() << "Generating dummy field" << std::endl;

    grib_handle * h = grib_handle_new_from_samples(NULL, "GRIB2");

    //grib_set_value(h, "decimalPrecision", 4);
    //grib_set_value(h, name_, value);
    //grib_write(h, outfile);

    // Extract the data buffer. n.b. Data is owned by gribapi (not copied)
    const void* buffer = NULL;
    size_t length = 0;

    GRIB_CALL(grib_get_message(h, &buffer, &length));
    eckit::Log::info() << "Message buffer length: " << length << std::endl;

    ASSERT(NULL != buffer);
    ASSERT(0 != length);

    // And pass the data to the sink.
    dataSink_.write(buffer, Length(length));
}

} // eckit
} // multiplexer

