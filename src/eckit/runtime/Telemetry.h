/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   Jan 2020

#ifndef eckit_runtime_Telemetry_h
#define eckit_runtime_Telemetry_h

#include <string>
#include <functional>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class JSON;

namespace runtime {

//----------------------------------------------------------------------------------------------------------------------

class Report {
public:
    enum Type
    {
        APPSTART = 0,
        APPSTOP,
        INFO,
        METER,
        COUNTER,
        MAXREPORT,
    };

public:
    virtual ~Report();

    virtual void json(JSON& s) const = 0;

    friend JSON& operator<<(eckit::JSON& s, const Report& r);
};

//----------------------------------------------------------------------------------------------------------------------


class Telemetry : public NonCopyable {
public:
    static void report(Report::Type);
    static void report(Report::Type, const Report&);
    static void report(Report::Type, std::function<void(JSON&)>);
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace runtime
} // namespace eckit

#endif
