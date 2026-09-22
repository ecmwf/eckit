// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @date   May 2019

#ifndef eckit_runtime_SessionID_H
#define eckit_runtime_SessionID_H

#include <sys/types.h>

#include <iosfwd>
#include <string>
#include <thread>

namespace eckit {

class Stream;
class Hash;

//----------------------------------------------------------------------------------------------------------------------

class SessionID {

public:  // methods

    SessionID();

    SessionID(Stream& s);

    bool operator==(const SessionID& rhs);
    bool operator!=(const SessionID& rhs);

    void hash(Hash& h) const;

    void encode(Stream& s) const;
    void print(std::ostream& os) const;

private:  // methods

    friend Stream& operator<<(Stream& s, const SessionID& si) {
        si.encode(s);
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const SessionID& si) {
        si.print(os);
        return os;
    }

private:  // members

    std::string hostname_;
    pid_t pid_;
    size_t thread_;
    time_t time_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif  // eckit_runtime_SessionID_H
