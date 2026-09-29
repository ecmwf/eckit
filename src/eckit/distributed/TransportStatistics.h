// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   TransportStatistics.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   April 2016

#ifndef eckit_TransportStatistics_H
#define eckit_TransportStatistics_H

#include <iosfwd>

#include "eckit/log/Statistics.h"

namespace eckit {
class Stream;
}

namespace eckit::distributed {

class TocHandler;

//----------------------------------------------------------------------------------------------------------------------

class TransportStatistics : public eckit::Statistics {
public:

    TransportStatistics();
    TransportStatistics(eckit::Stream&);


    size_t sendCount_;
    size_t receiveCount_;

    unsigned long long sendSize_;
    unsigned long long receiveSize_;

    eckit::Timing sendTiming_;
    eckit::Timing receiveTiming_;
    eckit::Timing barrierTiming_;
    eckit::Timing shutdownTiming_;


    TransportStatistics& operator+=(const TransportStatistics& other);
    TransportStatistics& operator/=(size_t);

    void report(std::ostream& out, const char* indent = "") const;

    void csvHeader(std::ostream& out) const;

    void csvRow(std::ostream& out) const;

    void encode(eckit::Stream&) const;

    friend eckit::Stream& operator<<(eckit::Stream& s, const TransportStatistics& x) {
        x.encode(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif
