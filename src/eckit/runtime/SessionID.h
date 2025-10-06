/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
