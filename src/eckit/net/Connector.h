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
/// @author Tiago Quintino
/// @date   Jun 2011

#ifndef eckit_net_Connector_h
#define eckit_net_Connector_h

#include "eckit/io/BufferCache.h"
#include "eckit/io/Length.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/net/TCPSocket.h"
#include "eckit/serialisation/Stream.h"


namespace eckit {
namespace net {

//----------------------------------------------------------------------------------------------------------------------

class ConnectorException : public Exception {
	virtual bool retryOnServer() const        { return true; }
	virtual bool retryOnClient() const        { return true; }
public:
	ConnectorException(const std::string& what) : Exception(what) {}
};

//----------------------------------------------------------------------------------------------------------------------

class Connector : public Stream {
public:

	Connector(const std::string& name, const std::string& node);

	~Connector();

// -- Methods

	void lock();
	void unlock();
    void reset();
    void check();

	bool locked() const { return locked_; }

	const std::string& host() const { return host_; }

    void autoclose(bool on) { autoclose_ = true; }

	void memoize(bool on, unsigned long time);

    static Connector& service(const std::string& name, const std::string& node);
    static Connector& service(const std::string& name, const std::map<std::string,Length>& cost);

    static NodeInfo nodeInfo(const std::string& name, const std::string& node);

protected:

// -- Members
	// None
	Connector(const std::string&, int);

// -- Methods

	void print(std::ostream&) const;

// -- Class members
	// None
    static Connector& get(const std::string& host, int port);


// -- Class methods
	// None

private:

// -- Members

    std::string host_;
    int port_;
    TCPSocket socket_;
    bool locked_;


	// Memoisation
    bool memoize_;
    bool sent_;
	unsigned long life_;

	BufferCache out_;
	BufferCache in_;
    bool autoclose_;

    std::map<BufferCache, BufferCache> cache_;

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
    virtual std::string name() const;

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const Connector& p)
		{ p.print(s); return s; }

    friend class ConnectorCache;

};


//----------------------------------------------------------------------------------------------------------------------


class AutoMemoize {
	Connector& c_;
	unsigned long  t_;
public:
	AutoMemoize(Connector& c,unsigned long t): c_(c), t_(t) { c_.memoize(true,t_); }
	~AutoMemoize() { c_.memoize(false,t_); }
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace net
} // namespace eckit

#endif
