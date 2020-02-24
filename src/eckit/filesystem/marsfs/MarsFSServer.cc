/*
 * (C) Copyright 1996- ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/marsfs/MarsFSServer.h"

#include "eckit/filesystem/marsfs/MarsFSService.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/Stream.h"

#include <fstream>

namespace eckit {

//-------------------------------------------------------------------------------------------------

MarsFSServer::MarsFSServer(int port):
    svc_(new MarsFSService(port)),
    marsfs_(svc_),
    lastExport_(0)
{
	marsfs_.start();
    heartbeat();
}

MarsFSServer::~MarsFSServer()
{
	marsfs_.stop();
}

int MarsFSServer::port() const
{
    return svc_->port();
}

void MarsFSServer::initialise(Stream& s) {
    last_.clear();
    refresh(s);
}

void MarsFSServer::refresh(Stream& s) {
    std::string reply;
    //Log::info() << "REFRESH ========== " << std::endl;
    bool force = false;
//    time_t now = ::time(0);

    eckit::PathName path("~/etc/disks/export");
    if(!path.exists())
        return;

    std::cout << "path.lastModified " << path.lastModified() << " " << lastExport_ << std::endl;
    if(path.lastModified() != lastExport_)
    {
        lastExport_ = path.lastModified();
        s << "forget";
        s >> reply;
        force = true;
    }

    std::ifstream in(path.localPath());

    std::string name, disk;

    while(in >> name) {

        PathName fs("~/etc/disks/" +  name);
        time_t last = fs.lastModified();
        std::map<std::string, time_t>::iterator j = last_.find(name);

        if(j == last_.end())
        {
            last_[name] = 0;
            j = last_.find(name);
        }

        if((last > (*j).second) || force)
        {
            s << "filespace";
            s << name;
            //Log::info() << name << std::endl;
            std::ifstream f(fs.localPath());
            while(f >> disk)
            {
                //Log::info() <<  disk << std::endl;
                s << disk;
            }
            s << "";
            s >> reply;

            last_[name] = last;
        }
    }

}


//-------------------------------------------------------------------------------------------------

} // namespace eckit
