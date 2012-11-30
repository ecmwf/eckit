/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Connector.cc
// Baudouin Raoult - (c) ECMWF Jun 11


#include "eclib/ClusterNodes.h"
#include "eclib/Connector.h"
#include "eclib/StrStream.h"
#include "eclib/TCPClient.h"
#include "eclib/TCPStream.h"
#include "eclib/ThreadSingleton.h"

Connector::Connector(const string& host, int port) :
	host_(host), port_(port), locked_(false), memoize_(false), sent_(false), life_(0)
{
	Log::info() << "Connector::Connector(" << host << "," << port << ")" << endl;
}

Connector::~Connector()
{
	static const char *here = __FUNCTION__;

	try
	{
		if (socket_.isConnected())
		{
			(*this) << "bye";
		}
	} catch (exception& e)
	{
		Log::error() << "** " << e.what() << " Caught in " << here << endl;
		Log::error() << "** Exception is ignored" << endl;
	}

}

TCPSocket& Connector::socket()
{

	static const char *here = __FUNCTION__;

	if (!socket_.isConnected())
	{
		try
		{
			NodeInfo remote;
			TCPClient client;
			Log::info() << "Connector::stream connecting to " << host_ << ":" << port_ << endl;
			socket_ = client.connect(host_, port_);
			InstantTCPStream s(socket_);

			// Login
			remote = NodeInfo::sendLogin(s);

            ClusterNodes::onLine(host_, port_);
		} catch (exception& e)
		{
			Log::error() << "** " << e.what() << " Caught in " << here << endl;
			Log::error() << "** Exception is handled" << endl;

            ClusterNodes::offLine(host_, port_);

            StrStream os;
			os << name() << ": " << e.what() << StrStream::ends;
			throw ConnectorException(string(os));
		}
	}
	return socket_;
}

void Connector::check()
{

	static const char *here = __FUNCTION__;

	if (socket_.isConnected())
	{
		try
		{
			if (!socket_.stillConnected())
			{
				socket_.close();
                ClusterNodes::offLine(host_, port_);
			}
		} catch (exception& e)
		{
			Log::error() << "** " << e.what() << " Caught in " << here << endl;
			Log::error() << "** Exception is handled" << endl;
			socket_.close();
            ClusterNodes::offLine(host_, port_);
		}
	}
}

void Connector::print(ostream&) const
{
}

class ConnectorCache {

	typedef multimap<pair<string, int> , Connector*> Cache;
	Cache cache_;

public:

	Connector& find(const string& host, int port)
	{
		pair<string, int> p(host, port);

		pair<Cache::iterator, Cache::iterator> r = cache_.equal_range(p);
		for (Cache::iterator j = r.first; j != r.second; ++j)
		{
			if (!((*j).second)->locked())
			{
				(*j).second->check();
				return *(*j).second;
			}
		}

		Connector* c = new Connector(host, port);
		cache_.insert(make_pair(p, c));
		return *c;
	}

	~ConnectorCache()
	{
		for (Cache::iterator j = cache_.begin(); j != cache_.end(); ++j)
		{
			Connector *c = (*j).second;
			delete c;
		}
	}

	void reset()
	{
		for (Cache::iterator j = cache_.begin(); j != cache_.end(); ++j)
		{
			Connector *c = (*j).second;
			c->reset();
		}
	}

};

class NodeInfoCache {

	typedef map<pair<string, string> , NodeInfo> Cache;
	Cache cache_;

public:

	NodeInfo& find(Stream& s, const string& name, const string& node)
	{
		pair<string, string> p(name, node);

		Cache::iterator j = cache_.find(p);
		if (j != cache_.end())
			return (*j).second;

		//Log::info() << "Connector::nodeInfo(" << name << "," << node << ")" << endl;

		s << "info";
		s << name;
		s << node;

		NodeInfo info;
		bool ok;

		s >> ok;

		if (!ok)
		{
			StrStream os;
			os << "Cannot get node info for " << name << "@" << node << StrStream::ends;
			throw ConnectorException(string(os));
		}

		s >> cache_[p];

		return cache_[p];
	}

	~NodeInfoCache()
	{
	}

	void reset()
	{
		cache_.clear();
	}

};

static ThreadSingleton<ConnectorCache> cache;
static ThreadSingleton<NodeInfoCache> infos;

Connector& Connector::get(const string& host, int port)
{
	//Log::info() << "Connector::get(" << host << "," << port << ")" << endl;
	return cache.instance().find(host, port);
}

Connector& Connector::service(const string& name, const string& node)
{
	//Log::info() << "Connector::service(" << name << "," << node << ")" << endl;
	NodeInfo info = ClusterNodes::lookUp(name, node);
	return get(info.host(), info.port());
}

Connector& Connector::service(const string& name, const map<string, Length>& cost)
{
	string host;
	int port = 0;
	Length best = 0;

	for (map<string, Length>::const_iterator j = cost.begin(); j != cost.end(); ++j)
	{
		if ((*j).second > best || port == 0)
		{
			best = (*j).second;
			if (ClusterNodes::available(name, (*j).first))
			{
				NodeInfo info = ClusterNodes::lookUp(name, (*j).first);
				host = info.host();
				port = info.port();
			}
            else {
                Log::warning() << "Service not available: " << name << "@"  << (*j).first <<endl;
            }
		}
	}

	if (!port)
	{
		NodeInfo info = ClusterNodes::any(name);
		host = info.host();
		port = info.port();
        Log::warning() << "Using node: " << info << endl;
	}

	ASSERT(port);

	return get(host, port);
}

void Connector::lock()
{
	ASSERT(!locked_);
	locked_ = true;
}

void Connector::unlock()
{
	ASSERT(locked_);
	locked_ = false;
	reset();
}

void Connector::reset()
{
	static const char *here = __FUNCTION__;

	in_.reset();
	out_.reset();
	cache_.clear();

	try
	{
		socket_.close();
	} catch (exception& e)
	{
		Log::error() << "** " << e.what() << " Caught in " << here << endl;
		Log::error() << "** Exception is ignored" << endl;
	}
}

string Connector::name() const
{
	StrStream os;
	os << "Connector[" << host_ << ":" << port_ << "]" << StrStream::ends;
	return string(os);
}

template<class T, class F>
long Connector::socketIo(F proc, T buf, long len, const char* msg)
{
	static const char *here = __FUNCTION__;
	TCPSocket& s = socket();
	long l = (s.*proc)(buf, len);
	if (l != len)
	{
		reset();
		cache.instance().reset();
		infos.instance().reset();
		StrStream os;
		os << "Connector::socketIo(" << name() << ") only " << l << " byte(s) " << msg << " intead of " << len << Log::syserr << StrStream::ends;
		//throw ConnectorException(string(os));
		throw Retry(string(os));
	}
	return l;
}

long Connector::write(const void *buf, long len)
{
	if (in_.count())
	{
		in_.reset();
		out_.count();
	}

	if (memoize_)
	{

		sent_ = false;
		out_.add(buf, len);

		return len;
	}

	return socketIo(&TCPSocket::write, buf, len, "written");
}

long Connector::read(void *buf, long len)
{
	if (memoize_)
	{
		if (!sent_)
		{
			map<BufferCache, BufferCache>::iterator j = cache_.find(out_);
			bool useCache = false;
			if (j != cache_.end())
			{
				//cout << "MEMOIZE IN CACHE " << (*j).first << endl;
				if ((::time(0) - (*j).second.updated()) > life_)
				{
					//cout << "  CACHE IS STALE" << (*j).first << endl;
				} else
				{
					useCache = true;
					cached_.buffer_ = (const char*) (*j).second.buffer();
					cached_.size_ = (*j).second.count();
					cached_.pos_ = 0;
					sent_ = true;
				}
			}

			if (!useCache)
			{
				cached_.buffer_ = 0;
				try
				{
					ASSERT(socketIo(&TCPSocket::write,out_.buffer(), out_.count(), "written") == out_.count());
				} catch (...)
				{
					reset();
					throw;
				}
				sent_ = true;
			}

		}

		if (cached_.buffer_)
		{

			long left = cached_.size_ - cached_.pos_;
			long l = left < len ? left : len;

			if (l != len)
			{
				StrStream os;
				os << "Connector::socketIo(" << name() << ") only " << l << " byte(s) memoized intead of " << len << Log::syserr << StrStream::ends;
				reset();
				throw ConnectorException(string(os));
			}

			::memcpy(buf, cached_.buffer_ + cached_.pos_, len);
			cached_.pos_ += len;

			return len;

		}
	}

	try
	{
		len = socketIo(&TCPSocket::read, buf, len, "read");
	} catch (...)
	{
		reset();
		throw;
	}

	if (memoize_)
	{
		ASSERT(len > 0);
		in_.add(buf, len);
	}

	return len;

}

void Connector::memoize(bool on, unsigned long life)
{
	ASSERT(on != memoize_);
	memoize_ = on;
	life_ = life;

	if (on)
	{
		ASSERT(in_.count() == 0);
		ASSERT(out_.count() == 0);
		sent_ = false;

		cached_.buffer_ = 0;

		if (cache_.size() > 10000)
		{
			//Log::info() << "Clear memoize cache" << endl;
			cache_.clear();
		}
	} else
	{
		//cout << "Connector::memoize " << in_.count() << " " << out_.count() << endl;
		//cout << "-> " << out_ << endl;
		if (cached_.buffer_)
		{
			//cout << "   CACHED" << endl;
			//cout << " .... " << cache_[out_] << endl;
		} else
		{
			//cout << "<- " << in_ << endl;

			cache_[out_] = in_;
		}
		in_.reset();
		out_.reset();

	}

}
