// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   Jan 2020

#ifndef eckit_runtime_Telemetry_h
#define eckit_runtime_Telemetry_h

#include <functional>
#include <string>

namespace eckit {

class JSON;

namespace runtime {

//----------------------------------------------------------------------------------------------------------------------

class Report {
public:

    enum Type {
        APPSTART = 0,
        APPSTOP,
        INFO,
        METER,
        COUNTER,
        KEEPALIVE,
        ENDTAG,
    };

public:

    virtual ~Report();

    virtual void json(JSON& s) const = 0;

    friend JSON& operator<<(eckit::JSON& s, const Report& r);
};

//----------------------------------------------------------------------------------------------------------------------


class Telemetry {
public:

    Telemetry() = delete;

    static std::string report(Report::Type);
    static std::string report(Report::Type, const Report&);
    static std::string report(Report::Type, std::function<void(JSON&)>);

    /// @returns total number of broadcasts sent so far (independent of the number of configured servers)
    /// Used for statistics and testing
    static unsigned long long countSent();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace runtime
}  // namespace eckit

#endif
