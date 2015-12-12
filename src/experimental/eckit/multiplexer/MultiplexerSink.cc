/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Simon Smart
/// @date Dec 2015

#include <iosfwd>
#include <fstream>

#include "eckit/multiplexer/MultiplexerSink.h"
#include "eckit/multiplexer/DataSink.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace multiplexer {


// ------------------------------------------------------------------------------------------------

MultiplexerSink::MultiplexerSink(const Configuration& config)
{
    const std::vector<LocalConfiguration> configs = config.getSubConfigurations("sinks");

    for(std::vector<LocalConfiguration>::const_iterator c = configs.begin(); c != configs.end(); ++c) {
        sinks_.push_back( DataSinkFactory::build(c->getString("sinkName"),*c) );
    }
}

MultiplexerSink::~MultiplexerSink() {

    close();

    for(sink_store_t::iterator it = sinks_.begin(); it != sinks_.end(); ++it) {
        delete (*it);
    }
}


void MultiplexerSink::open(const std::string& key) {

    eckit::Log::info() << "[" << *this << "]: open" << std::endl;

    for(sink_store_t::iterator it = sinks_.begin(); it != sinks_.end(); ++it) {
        (*it)->open(key);
    }
}


void MultiplexerSink::write(const void* buffer, const Length& length) {

    eckit::Log::info() << "[" << *this << "]: write (" << length << ")" << std::endl;

    for(sink_store_t::iterator it = sinks_.begin(); it != sinks_.end(); ++it) {
        (*it)->write(buffer, length);
    }
}


void MultiplexerSink::close() {

    eckit::Log::info() << "[" << *this << "]: close" << std::endl;

    for(sink_store_t::iterator it = sinks_.begin(); it != sinks_.end(); ++it) {
        (*it)->close();
    }
}

void MultiplexerSink::print(std::ostream& os) const {
    os << "MultiplexerSink(";
    for(sink_store_t::const_iterator it = sinks_.begin(); it != sinks_.end(); ++it) {
        os << (*it);
    }
    os << ")";
}

DataSinkBuilder<MultiplexerSink> MultiplexerSinkBuilder("multiplexer");

// ------------------------------------------------------------------------------------------------

}  // namespace multiplexer
}  // namespace eckit

