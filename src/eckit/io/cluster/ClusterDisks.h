// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @date   Jun 2011
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_ClusterDisks_h
#define eckit_ClusterDisks_h

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/JSON.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class NodeInfo;
class JSON;

class ClusterDisks {
public:

    static void reset();
    static void cleanup();

    static void offLine(const NodeInfo&);
    static void forget(const NodeInfo&);
    static void update(const std::string&, const std::string&, const std::vector<std::string>&);

    static void list(std::ostream& out);
    static void json(JSON& out);

    static void send(Stream& s);
    static void receive(Stream& s);

    static time_t lastModified(const std::string&);
    static void load(const std::string&, std::vector<std::string>&);
    static std::string node(const std::string& path);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
