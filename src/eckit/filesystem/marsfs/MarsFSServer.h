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
/// @author Simon Smart

#ifndef eckit_filesystem_marsfs_MarsFSServer_h
#define eckit_filesystem_marsfs_MarsFSServer_h

#include "eckit/net/NetService.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/io/cluster/ClusterNode.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

class MarsFSServer : public eckit::ClusterNode, private eckit::NonCopyable {
public:

	MarsFSServer(int port);

	~MarsFSServer();

private: // methods

    int port() const override;
    void initialise(eckit::Stream&) override;
    void refresh(eckit::Stream&) override;

private: // members

    eckit::net::NetService* svc_;
	eckit::ThreadControler marsfs_;

    std::map<std::string, time_t> last_;
    time_t lastExport_;
};

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
