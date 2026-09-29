// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File ClusterNodes.h
// Baudouin Raoult - (c) ECMWF Jul 11

#ifndef eckit_ClusterNodes_h
#define eckit_ClusterNodes_h

#include "eckit/eckit.h"

#include "eckit/io/cluster/NodeInfo.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;
class JSON;

class ClusterNodes {
public:

    // -- Class methods
    // None

    static void reset();
    static void cleanup();
    static void forget(const NodeInfo&);
    static void offLine(const NodeInfo&);
    static void refresh(const NodeInfo&);
    static void list(std::ostream& out);
    static void json(JSON& out);

    static void send(Stream& s);
    static void receive(Stream& s);

    static NodeInfo lookUp(const std::string& type, const std::string& node);

    // Attributes argument is optional. Determines a labelled subset of a given type of node
    static NodeInfo any(const std::string& type, const std::set<std::string>& attributes = {});

    static bool available(const std::string&, const std::string&);
    static bool offsite(const std::string&, const std::string&);

    static void offLine(const std::string&, int);
    static void onLine(const std::string&, int);

    static std::vector<NodeInfo> all();

    static bool lookUpHost(const std::string& type, const std::string& host, NodeInfo&);
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
