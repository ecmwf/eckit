/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Comm.h"

#include <map>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

Comm& comm(const char* name)
{
    return Comm::comm(name);
}

bool isRunning()
{
    return Comm::areMPIVarsSet();
}

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;
static eckit::Mutex *local_mutex = 0;
static std::map<std::string, CommFactory*>* factories = 0;
static std::map<std::string, Comm*>*        communicators = 0;

static Comm* defaultComm = 0;

static void init() {
    local_mutex   = new eckit::Mutex();
    factories     = new std::map<std::string, CommFactory*>();
    communicators = new std::map<std::string, Comm*>();
}

//----------------------------------------------------------------------------------------------------------------------

CommFactory::CommFactory(const std::string &name):
    name_(name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(factories->find(name) == factories->end());
    (*factories)[name] = this;
}

CommFactory::~CommFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    factories->erase(name_);
}

void CommFactory::list(std::ostream& out) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (std::map<std::string, CommFactory *>::const_iterator j = factories->begin() ; j != factories->end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}

Comm* CommFactory::build(const std::string &name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, CommFactory *>::const_iterator j = factories->find(name);

    eckit::Log::debug<LibEcKit>() << "Looking for CommFactory [" << name << "]" << std::endl;

    if (j == factories->end()) {
        eckit::Log::error() << "No CommFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "CommFactories are:" << std::endl;
        for (j = factories->begin() ; j != factories->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No CommFactory called ") + name);
    }

    return (*j).second->make();
}

//----------------------------------------------------------------------------------------------------------------------

Comm& Comm::comm(const char* name)
{
    if(!name && defaultComm) {
        return *defaultComm;
    }

    if(!name && !defaultComm) {
        initDefaultComm();
        return *defaultComm;
    }

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    std::map<std::string,Comm*>::iterator itr = communicators->find(name);

    if(itr != communicators->end()) {
        return *itr->second;
    }

    eckit::Log::error() << "No Communicator '" << name << "'" << std::endl;
    eckit::Log::error() << "Current communicators are:" << std::endl;
    for(itr = communicators->begin() ; itr != communicators->end() ; ++itr)
        eckit::Log::error() << "   " << (*itr).first << std::endl;
    throw eckit::SeriousBug(std::string("No communicator called ") + name);
}

bool Comm::areMPIVarsSet()
{
    return (::getenv("OMPI_COMM_WORLD_SIZE") || ::getenv("ALPS_APP_PE"));
}

void Comm::initDefaultComm()
{
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(!defaultComm);

    if(areMPIVarsSet()) {
        defaultComm = CommFactory::build("parallel");
    }
    else {
        defaultComm = CommFactory::build("serial");
    }

    (*communicators)["world"] = defaultComm;
}

#if 0
Comm& Comm::create(const char* name)
{
    Comm* comm;    
    ///
    (*communicators)[name] = comm;
}
#endif

//----------------------------------------------------------------------------------------------------------------------

Comm::Comm() {
}

Comm::~Comm() {
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
