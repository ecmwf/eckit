/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Connector.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_Connector_h
#define eckit_Connector_h

#include "eckit/io/BufferCache.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/io/Length.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ConnectorException : public Exception {
	virtual bool retryOnServer() const        { return true; }
	virtual bool retryOnClient() const        { return true; }
public:
	ConnectorException(const string& what) : Exception(what) {}
};

//-----------------------------------------------------------------------------

class Connector : public Stream {
public:

// -- Exceptions
	// None

// -- Contructors

	Connector(const string& name, const string& node);

// -- Destructor

	~Connector(); 

// -- Methods

	void lock();
	void unlock();
    void reset();
    void check();

	bool locked() const { return locked_; }

	const string& host() const { return host_; }

	void memoize(bool on, unsigned long time);

// -- Class methods
	// None

    static Connector& service(const string& name, const string& node);
    static Connector& service(const string& name, const map<string,Length>& cost);
    /* static Connector& clusterNode(); */

    static NodeInfo nodeInfo(const string& name, const string& node);

protected:

// -- Members
	// None
	Connector(const string&, int);

// -- Methods
	
	void print(ostream&) const; 	

// -- Class members
	// None
    static Connector& get(const string& host, int port);


// -- Class methods
	// None

private:

// -- Members

    string host_;
    int port_;
    TCPSocket socket_;
    bool locked_;


	// Memoisation 
    bool memoize_;
    bool sent_;
	unsigned long life_;

	BufferCache out_;
	BufferCache in_;

	map<BufferCache, BufferCache> cache_;

	struct {
		const char *buffer_;
		size_t pos_;
		size_t size_;
	} cached_;

// -- Methods
	// None

    TCPSocket& socket();
    template<class T, class F> long socketIo(F proc, T buf, long len, const char*);

// -- Overridden methods
	// None
    
    // From Stream
	virtual long write(const void* buf,long len) ;
	virtual long read(void* buf,long len);
    virtual string name() const;

// -- Friends

	friend ostream& operator<<(ostream& s,const Connector& p)
		{ p.print(s); return s; }

    friend class ConnectorCache;

};

class AutoMemoize {
	Connector& c_;
	unsigned long  t_;
public:
	AutoMemoize(Connector& c,unsigned long t): c_(c), t_(t) { c_.memoize(true,t_); }
	~AutoMemoize() { c_.memoize(false,t_); }
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
