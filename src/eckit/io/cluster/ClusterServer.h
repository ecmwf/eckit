/*
 * (C) Copyright 1996- ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult (Jun 96)
/// @author Simon Smart (Feb 2020)

#ifndef eckit_io_cluster_ClusterServer_H
#define eckit_io_cluster_ClusterServer_H

#include "eckit/thread/ThreadControler.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

class ClusterServer {

public: // methods

	ClusterServer(int port);
	~ClusterServer();

    // No copy allowed
    ClusterServer(const ClusterServer&) = delete;
    ClusterServer& operator=(const ClusterServer&) = delete;

private: // members

	eckit::ThreadControler cluster_;
};

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
