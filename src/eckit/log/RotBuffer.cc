/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/RotBuffer.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Context.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

namespace {

static StaticMutex local_mutex;

static std::ofstream* last     = 0;
static time_t   lastTime  = 0;

}  // anonymous namespace

//-----------------------------------------------------------------------------

RotBuffer::RotBuffer() : ChannelBuffer() {}

RotBuffer::~RotBuffer() {
    dumpBuffer();
}

static std::ostream& rotout() {
    time_t now = ::time(0) / 86400;

    if(now != lastTime || last == 0) {
        static std::string logfileFormat = Resource<std::string>("logfileFormat","~/log/%Y-%m-%d/out");

        TimeStamp ts(logfileFormat);
        PathName path(ts);
        path.mkdir(0777);

        std::ostringstream os;
        os << path  << "/" << Context::instance().runName();

        delete last;

        /// @todo Find a way to set the close on exec flags
        last = new std::ofstream(os.str().c_str(), std::ios::app);

        lastTime = now;
    }

    return *last;
}

bool RotBuffer::dumpBuffer() {
    /// @note protects data-race from multiple threads logging
    ///       since it is the only function that accesses the resource
    AutoLock<StaticMutex> lock(local_mutex);

    std::ostream& out = rotout();

    out.write(pbase(),pptr() - pbase());

    setp(pbase(), epptr());

    out.flush(); // flush here so sync() does not need to use rotout()

    return true;
}

std::streambuf::int_type RotBuffer::overflow(std::streambuf::int_type ch) {
    if (ch == traits_type::eof() )
        return sync();
    dumpBuffer();
    sputc(ch);
    return traits_type::to_int_type(ch);
}

std::streambuf::int_type RotBuffer::sync() {
    return ( dumpBuffer() ? 0 : -1 );
}

//-----------------------------------------------------------------------------

} // namespace eckit
