/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Hour.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {

class BadHour : public BadValue {
public:
    BadHour(const std::string& s) :
        BadValue(s) {}
};

Hour::Hour(double hour) {
    if (hour < 0) {
        std::ostringstream oss;
        oss << "Invalid hour '" << hour << "'";
        throw BadHour(oss.str());
    }
    seconds_ = long(hour * 3600 + 0.5);
}

Hour::Hour(const std::string& s) {
    Tokenizer parse(":");
    std::vector<std::string> result;
    Translator<std::string, long> s2l;

    parse(s, result);
    bool error = false;

    if (result.size() > 3) {
        error = true;
    }

    if (result.size() > 2) {
        double x = s2l(result[2]);
        if (x < 0 || x >= 60) {
            error = true;
        }
        seconds_ += x;
    }

    if (result.size() > 1) {
        double x = s2l(result[1]);
        if (x < 0 || x >= 60) {
            error = true;
        }
        seconds_ += x * 60;
    }

    {
        double x = s2l(result[0]);
        if (x < 0) {
            error = true;
        }
        seconds_ += x * 3600;
    }


    if (error) {
        throw BadHour(std::string("Invalid hour '") + s + "'");
    }
}

std::string Hour::asString() const {
    std::ostringstream oss;
    print(oss);
    return oss.str();
}

void Hour::print(std::ostream& out) const {
    long seconds = seconds_ % 60;
    long minutes = (seconds_ / 60) % 60;
    long hours   = seconds_ / 3600;

    if (seconds) {
        out << hours << ':' << std::setw(2) << std::setfill('0') << minutes << ':' << std::setw(2) << std::setfill('0')
            << seconds;
        return;
    }

    if (minutes) {
        out << hours << ':' << std::setw(2) << std::setfill('0') << minutes;
        return;
    }

    out << hours;
}


}  // namespace eckit
