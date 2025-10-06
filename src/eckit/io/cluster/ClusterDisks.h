/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
