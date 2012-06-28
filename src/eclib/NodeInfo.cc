/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <pwd.h>

#include "eclib/Application.h"
#include "eclib/AutoLock.h"
#include "eclib/Monitor.h"
#include "eclib/Mutex.h"
#include "eclib/NodeInfo.h"
#include "eclib/Resource.h"
#include "eclib/StrStream.h"
#include "eclib/ThreadSingleton.h"

CREATE_MUTEX();

NodeInfo::NodeInfo() :
	port_(0), id_(0), task_(-1)
{
}

NodeInfo& NodeInfo::init()
{
    INIT_MUTEX();
    AutoLock<Mutex> lock(mutex);
	if(!name_.length())
	{
        static string myNode = Resource<string> ("node", "<missing-node-name>");
        static string myHost = Resource<string> ("host", "");
        static string myUser;


		name_ = Application::appName();

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

		task_ = Monitor::self();
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
	string p;
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

	MarsID id;
	s >> id;
	info.id(id);

	long task;
	s >> task;
	info.task(task);
}

void NodeInfo::print(ostream& s) const
{
	s << "[" << name_ << ":" << setfill('0') << setw(3) << task_ << setfill(' ') << "," << node_ << "@" << host_ << ":" << port_ << "," << user_ << "]";
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
		StrStream os;
		os << "User mismatch: " << here << " " << remote << StrStream::ends;
		s << Exception(string(os));
	}

	Log::info() << "Connection established " << here << " <=> " << remote << endl;

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
		StrStream os;
		os << "User mismatch: " << here << " " << remote << StrStream::ends;
		s << Exception(string(os));
	}

	Log::info() << "Connection established " << here << " <=> " << remote << endl;

	return remote;
}
