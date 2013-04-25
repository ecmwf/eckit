/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClusterDisks.h
// Baudouin Raoult - (c) ECMWF Jul 11

#ifndef eckit_ClusterDisks_h
#define eckit_ClusterDisks_h

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class NodeInfo;
class JSON;

class ClusterDisks {
public:



	static void reset();
	static void cleanup();

	static void offLine(const NodeInfo&);
	static void forget(const NodeInfo&);
	static void update(const string&,const string&, const vector<string>&);

	static void list(ostream& out);
    static void json(JSON& out);


    static time_t lastModified(const string&);
    static void   load(const string&, vector<string>&);
    static string node(const string& path);


};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
