// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_ClusterNode_h
#define eckit_ClusterNode_h

#include <set>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class NodeInfo;

class ClusterNode {
public:

    ClusterNode();

    ClusterNode(const ClusterNode&)            = delete;
    ClusterNode& operator=(const ClusterNode&) = delete;
    ClusterNode(ClusterNode&&)                 = delete;
    ClusterNode& operator=(ClusterNode&&)      = delete;

    virtual ~ClusterNode();

    void heartbeat();

    virtual const std::set<std::string>& attributes() const;
    virtual int port() const         = 0;
    virtual void initialise(Stream&) = 0;
    virtual void refresh(Stream&)    = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
