/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <pwd.h>

#include "eckit/thread/AutoLock.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/config/Resource.h"
#include "eckit/thread/ThreadSingleton.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Once<Mutex> local_mutex;

NodeInfo::NodeInfo() :
	port_(0), active_(false), id_(0), task_(-1)
{
}

NodeInfo& NodeInfo::init()
{
    AutoLock<Mutex> lock(local_mutex);
	if(!name_.length())
	{
        static std::string myNode = Resource<std::string> ("node", "<missing-node-name>");
        static std::string myHost = Resource<std::string> ("host", "");
        static std::string myUser;

		name_ = Main::instance().name();

		host_ = myHost;
		if(host_.length() == 0)
		{
			char host[1024];
			SYSCALL(gethostname(host,sizeof(host)-1));
			host_ = host;
			host_ = host_.substr(0, host_.find("."));
            myHost = host;
		}

		node_ = myNode;

		if(node_.length() == 0)
        {
			node_  = host_;
            myNode = node_;
        }

        user_ = myUser;

        if(user_.length() == 0) {

            user_ = "<nobody>";
            char buf[4096];
            struct passwd pwbuf;
            struct passwd *pwbufp = 0;
            SYSCALL(getpwuid_r(getuid(), &pwbuf, buf, sizeof(buf), &pwbufp));
            if(pwbufp)
            {
                user_ = pwbuf.pw_name;
            }

            myUser = user_;
        }

		task_ = Monitor::instance().self();
	}
	return *this;
}

NodeInfo::~NodeInfo()
{
}

static ThreadSingleton<NodeInfo> n;

NodeInfo& NodeInfo::thisNode()
{
	return n.instance().init();
}

void operator<<(Stream& s, const NodeInfo& info)
{
	s << info.user();
	s << info.name();
	s << info.node();
	s << info.host();
	s << info.port();
	s << info.id();
	s << info.task();
}

void operator>>(Stream& s, NodeInfo& info)
{
	std::string p;
	int l;
	s >> p;
	info.user(p);
	s >> p;
	info.name(p);
	s >> p;
	info.node(p);
	s >> p;
	info.host(p);
	s >> l;
	info.port(l);

	TaskID id;
	s >> id;
	info.id(id);

	long task;
	s >> task;
	info.task(task);
}

void NodeInfo::print(std::ostream& s) const
{
    s << "[" << name_ << ":" << std::setfill('0') << std::setw(3) << task_ << std::setfill(' ') << "," << node_ << "@" << host_ << ":" << port_ << "," << user_ << "]";
}

NodeInfo NodeInfo::acceptLogin(Stream& s)
{
	NodeInfo remote;
	NodeInfo& here = thisNode();

	s >> remote;
	if(here.user() == remote.user())
	{
		s << here;
	}
	else
	{
        std::ostringstream os;
        os << "User mismatch: " << here << " " << remote;
        s << Exception(os.str());
	}

	Log::info() << "Connection established " << here << " <=> " << remote << std::endl;

	return remote;
}

NodeInfo NodeInfo::sendLogin(Stream& s)
{
	NodeInfo remote;
	NodeInfo& here = thisNode();
	s << here;
	s >> remote;

	if(here.user() != remote.user())
	{
        std::ostringstream os;
        os << "User mismatch: " << here << " " << remote;
        s << Exception(os.str());
	}

	Log::info() << "Connection established " << here << " <=> " << remote << std::endl;

	return remote;
}

//-----------------------------------------------------------------------------

} // namespace eckit

