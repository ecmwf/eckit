/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NodeInfo.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef NodeInfo_H
#define NodeInfo_H

#include "eclib/machine.h"
#include "eclib/Types.h"

class NodeInfo {
public:

// -- Contructors

	NodeInfo();

// -- Destructor

	~NodeInfo();

// -- Methods
//

	void  port(int p)    { port_ = p; }
	int   port() const   { return port_; }

	void  active(bool a)  { active_ = a; }
	bool active() const   { return active_; }

	void     host(const string& h)    { host_ = h; }
	const string&   host() const             { return host_; }

	void     name(const string& h)    { name_ = h; }
	const string&    name() const             { return name_; }

	void     user(const string& h)    { user_ = h;}
	const string&    user() const             { return user_; }

	void     node(const string& h)    { node_ = h; }
	const string&    node() const             { return node_; }


	void  id(MarsID p)    { id_ = p; }
	MarsID   id() const   { return id_; }

	void  task(long p)    { task_ = p; }
	long   task() const   { return task_; }


	NodeInfo& init();

    static NodeInfo& thisNode();
    static NodeInfo acceptLogin(Stream&);
    static NodeInfo sendLogin(Stream&);



private:

// No copy allowed

	//NodeInfo(const NodeInfo&);
	//NodeInfo& operator=(const NodeInfo&);

	string name_;
	string node_;
	string user_;
	string host_;
	int    port_;
    bool   active_;
	MarsID id_;
	long   task_;

// -- Methods

	void print(ostream&) const; 

// -- Friends

	friend ostream& operator<<(ostream& s,const NodeInfo& p)
		{ p.print(s); return s; }

	friend void operator<<(Stream&,const NodeInfo&);
	friend void operator>>(Stream&,NodeInfo&);
};

// Used by MappedArray

inline unsigned long version(NodeInfo*) { return 1; }

#endif
