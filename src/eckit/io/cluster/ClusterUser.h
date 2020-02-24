/*
 * (C) Copyright 1996- ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult (Feb 2002)
/// @author Simon Smart (Feb 2020)

#ifndef eckit_io_cluster_ClusterUser_h
#define eckit_io_cluster_ClusterUser_h

#include "eckit/net/NetUser.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

class ClusterUser : public eckit::net::NetUser {

public: // methods

	ClusterUser(eckit::net::TCPSocket&);
	~ClusterUser(); 

    // No copy allowed
    ClusterUser(const ClusterUser&) = delete;
    ClusterUser& operator=(const ClusterUser&) = delete;

private:

    void serve(eckit::Stream&, std::istream&,std::ostream&) override;
};

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
