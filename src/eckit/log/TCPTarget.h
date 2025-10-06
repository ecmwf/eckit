/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   TCPTarget.h
/// @author Metin Cakircali
/// @date   May 2025

#include <iosfwd>
#include <string>

#include "eckit/log/LogTarget.h"
#include "eckit/net/TCPSocket.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief sends log messages over a TCP
class TCPTarget : public LogTarget {

public:  // methods

    /// @note takes ownership of [socket]
    /// @param socket TCP socket to use for log messages
    explicit TCPTarget(net::TCPSocket& socket);

    // rules
    TCPTarget(const TCPTarget&)            = delete;
    TCPTarget& operator=(const TCPTarget&) = delete;
    TCPTarget(TCPTarget&&)                 = delete;
    TCPTarget& operator=(TCPTarget&&)      = delete;

    ~TCPTarget() override = default;

    void write(const char* start, const char* end) override;

    void flush() override;

protected:  // methods

    void print(std::ostream& out) const override;

    void write(const std::string& msg);

    const net::TCPSocket& socket() const { return out_; }

private:  // members

    net::TCPSocket out_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
