// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jul 96

#ifndef eckit_log_TimeStamp_h
#define eckit_log_TimeStamp_h

#include <ctime>

#include <iosfwd>
#include <string>

namespace eckit {


class TimeStamp {

public:  // methods

    TimeStamp(const std::string& = defaultFormat_);
    TimeStamp(time_t, const std::string& = defaultFormat_);

    operator std::string() const;

    friend std::ostream& operator<<(std::ostream&, const TimeStamp&);

private:  // members

    time_t time_;
    const std::string& format_;

    static const char* defaultFormat_;
};


}  // namespace eckit

#endif
