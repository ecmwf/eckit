/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClusterNode.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_ClusterNode_h
#define eckit_ClusterNode_h

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;
class NodeInfo;

class ClusterNode {
public:

// -- Contructors

	ClusterNode();

// -- Destructor

	virtual ~ClusterNode();

// -- Methods

	void heartbeat();

// -- For cluster MARS

	virtual int port() const = 0;
	virtual void initialise(Stream&)  = 0;
	virtual void refresh(Stream&)  = 0;

// -- Overridden methods


// -- Class methods

	//static Stream& connector(const std::string& name, const std::string& node);


//	static bool         exited()    { return instance_ != 0; }

protected:


// -- Overridden methods

	// From Configurable

// -- Class methods

private:

// No copy allowed

	ClusterNode(const ClusterNode&);
	ClusterNode& operator=(const ClusterNode&);

// -- Members


// -- Overridden methods

	// From Configurable


// -- Class members

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
