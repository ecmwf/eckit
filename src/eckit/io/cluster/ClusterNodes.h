/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClusterNodes.h
// Baudouin Raoult - (c) ECMWF Jul 11

#ifndef eckit_ClusterNodes_h
#define eckit_ClusterNodes_h

#include "eckit/machine.h"

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
    static void list(ostream& out);
    static void json(JSON& out);

    static void send(Stream& s);
    static void receive(Stream& s);

    static NodeInfo lookUp(const string&, const string&);
    static NodeInfo any(const string&);

    static bool available(const string&, const string&);

	static void offLine(const string&, int);
	static void onLine(const string&, int);

    static vector<NodeInfo> all();

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
