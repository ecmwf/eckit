/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
