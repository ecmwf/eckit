/*
 * (C) Copyright 1996- ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/cluster//ClusterUser.h"

#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/thread/Mutex.h"


namespace eckit {

//-------------------------------------------------------------------------------------------------

ClusterUser::ClusterUser(net::TCPSocket& protocol) :
    NetUser(protocol) {}

ClusterUser::~ClusterUser() {}

void ClusterUser::serve(eckit::Stream& s, std::istream& in,std::ostream& out)
{
	std::string cmd;

	std::string ok("ok");

	char hb[] = ".:";
	int n = 0;

    std::string myNode = NodeInfo::thisNode().node();

	NodeInfo info;


	try {


		info = NodeInfo::acceptLogin(s);

		Log::info() << "Get connection from " << info << std::endl;

		Monitor::instance().name(info.name());
		Log::message() << info.node() << std::endl;
		Log::status()  << info << std::endl;


		for(;;) {
			s >> cmd;

			if(cmd == "heartbeat") {
				Monitor::instance().state(hb[n]); n = 1-n;

				ClusterNodes::refresh(info);

                // Don't create deadlocks
                if(info.node() != myNode) 
                {
                    s << "sync";
                    ClusterNodes::send(s);
                }
                else
                {
                    s << ok;
                }
				continue;
			}

			Monitor::instance().state(cmd.length() > 0 ? cmd[0] : ' ');
			Log::info() << "CMD [" << cmd << "]" <<  " " << info << std::endl;

			if(cmd == "forget") {
				s << ok;
				ClusterDisks::forget(info);
				continue;
			}

			if(cmd == "filespace") {
				std::vector<std::string> disks;

				std::string name, disk;
				s >> name;

				for(;;) {
					s >> disk;
					Log::info() << "FILESPACE " << name << " -> " << disk << std::endl;
					if(disk.length() == 0)
						break;
					disks.push_back(disk);
				}

				s << ok;

				ClusterDisks::update(info.node() ,name, disks);

				continue;
			}



			if(cmd == "bye") {
				break;
			}

			throw eckit::UserError(std::string("Invalid command [" + cmd + "]"));
		}
	}
	catch(std::exception& e) {
		try {
			Log::error() << "** " << e.what() << " Caught in " << Here() <<  std::endl;
			Log::error() << "** Exception is handled" << std::endl;

			s << e;
		}
		catch(std::exception& a)
		{
			Log::error() << "** " << a.what() << " Caught in " << Here() <<  std::endl;
			Log::error() << "** Exception is ignored" << std::endl;
		}
	}


    ClusterNodes::offLine(info);
    ClusterDisks::offLine(info);

}

//-------------------------------------------------------------------------------------------------

} // namespace eckit
