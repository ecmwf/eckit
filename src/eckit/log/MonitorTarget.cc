/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/MonitorTarget.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MonitorTarget::MonitorTarget(LogTarget* target) : WrapperTarget(target)
{
}

MonitorTarget::~MonitorTarget()
{
}

void MonitorTarget::write(const char* start, const char* end)
{
    Monitor::instance().out(const_cast<char*>(start), const_cast<char*>(end));
    target_->write(start, end);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
