/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult (Oct 96)
/// @author Simon Smart (Feb 2020)

#ifndef eckit_filesystem_marsfs_MarsFSService_h
#define eckit_filesystem_marsfs_MarsFSService_h

#include "eckit/net/NetService.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

class MarsFSService : public eckit::net::NetService {

public: // methods

	MarsFSService(int);
	~MarsFSService();

    // No copy allowed
    MarsFSService(const MarsFSService&) = delete;
    MarsFSService& operator=(const MarsFSService&) = delete;

private: // methods

    eckit::net::NetUser* newUser(eckit::net::TCPSocket&) const override;
    std::string name() const override { return "marsfs"; }
    bool preferToRunAsProcess() const override;
};

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
