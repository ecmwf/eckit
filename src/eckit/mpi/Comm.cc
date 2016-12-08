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

class Environment {
public:

    static bool areMPIVarsSet() {
        return (
          !::getenv("ECKIT_MPI_FORCE_SERIAL") &&   // force serial communicator
          (::getenv("OMPI_COMM_WORLD_SIZE")   ||   // OpenMPI
           ::getenv("ALPS_APP_PE")            ||   // Cray PE
           ::getenv("PMI_SIZE")               ||   // Intel
           ::getenv("ECKIT_MPI_FORCE_PARALLEL"))); // If all else fails
    }

    static Environment& instance() {
        static Environment env;
        return env;
    }

    void initDefault() {

        AutoLock<Mutex> lock(mutex_);

        ASSERT(!default_);

        Comm* world;

        if(areMPIVarsSet()) {
            world = CommFactory::build("parallel");
        }
        else {
            world = CommFactory::build("serial");
        }

        communicators["world"] = world;
        communicators["self"]  = world->self();

        default_ = world;
    }

    void setDefaut(const char* name) {

        AutoLock<Mutex> lock(mutex_);

        std::map<std::string, Comm*>::iterator itr = communicators.find(name);
        if(itr != communicators.end()) {
            default_ = itr->second;
            return;
        }

        eckit::Log::error() << "Cannot set default communicator to '" << name << "', no communicator with that name was found" << std::endl;
        eckit::Log::error() << "Current communicators are:" << std::endl;
        for(itr = communicators.begin() ; itr != communicators.end() ; ++itr)
            eckit::Log::error() << "   " << (*itr).first << std::endl;
        throw eckit::SeriousBug(std::string("No communicator called ") + name);
    }

    bool hasComm(const char* name) {
        AutoLock<Mutex> lock(mutex_);
        std::map<std::string, Comm*>::iterator itr = communicators.find(name);
        if(itr != communicators.end()) {
            return true;
        }
        return false;
    }

    Comm& getComm(const char* name = 0) {

        AutoLock<Mutex> lock(mutex_);

        if(!name && default_) return *default_; /* most common case first */

        if(!default_) { initDefault(); }

        if(!name) {
            ASSERT(default_);  /* sanity check init was successful */
            return *default_;
        }

        std::map<std::string, Comm*>::iterator itr = communicators.find(name);
        if(itr != communicators.end()) {
            return *itr->second;
        }

        eckit::Log::error() << "No Communicator '" << name << "'" << std::endl;
        eckit::Log::error() << "Current communicators are:" << std::endl;
        for(itr = communicators.begin() ; itr != communicators.end() ; ++itr)
            eckit::Log::error() << "   " << (*itr).first << std::endl;
        throw eckit::SeriousBug(std::string("No communicator called ") + name);
    }

    void addComm(const char* name, int comm) {

        AutoLock<Mutex> lock(mutex_);

        std::map<std::string, Comm*>::iterator itr = communicators.find(name);
        if(itr != communicators.end()) {
            eckit::Log::error() << "Cannot create communicator '" << name << "', communicator with that name already exists" << std::endl;
            eckit::Log::error() << "Current communicators are:" << std::endl;
            for(itr = communicators.begin() ; itr != communicators.end() ; ++itr)
                eckit::Log::error() << "   " << (*itr).first << std::endl;
            throw eckit::SeriousBug(std::string("Communicator alredy exists: ") + name);
        }

        Comm* pComm = 0;
        if(areMPIVarsSet()) {
            pComm = CommFactory::build("parallel", comm);
        }
        else {
            pComm = CommFactory::build("serial", comm);
        }

        communicators[name] = pComm;
    }

    CommFactory& getFactory(const std::string& name) {

        AutoLock<Mutex> lock(mutex_);

        std::map<std::string, CommFactory *>::const_iterator j = factories.find(name);

        if (j != factories.end()) { return *(j->second); }

        eckit::Log::error() << "No CommFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "CommFactories are:" << std::endl;
        for (j = factories.begin() ; j != factories.end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;

        throw eckit::SeriousBug(std::string("No CommFactory called ") + name);
    }

    void registFactory(const std::string& name, CommFactory* f) {
        AutoLock<Mutex> lock(mutex_);
        ASSERT(factories.find(name) == factories.end());
        factories[name] = f;
    }

    void unregistFactory(const std::string& name) {
        AutoLock<Mutex> lock(mutex_);
        factories.erase(name);
    }

    Environment() : default_(0) {}

    ~Environment() {

        AutoLock<Mutex> lock(mutex_);

        if(default_) {
            for(std::map<std::string,Comm*>::iterator itr = communicators.begin() ; itr != communicators.end() ; ++itr) {
                delete itr->second;
            }
            default_ = 0;
        }
    }

    Comm* default_;

    std::map<std::string, Comm*> communicators;

    std::map<std::string, CommFactory*> factories;

    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

CommFactory::CommFactory(const std::string &name):
    name_(name) {
    Environment::instance().registFactory(name, this);
}

CommFactory::~CommFactory() {
    Environment::instance().unregistFactory(name_);
}

Comm* CommFactory::build(const std::string& name) {
    return Environment::instance().getFactory(name).make();
}

Comm*CommFactory::build(const std::string& name, int comm)
{
    return Environment::instance().getFactory(name).make(comm);
}

//----------------------------------------------------------------------------------------------------------------------

Comm::Comm() {
}

Comm::~Comm() {
}

//----------------------------------------------------------------------------------------------------------------------

Comm& comm(const char* name) {
    return Environment::instance().getComm(name);
}

void setCommDefault(const char* name)
{
    Environment::instance().setDefaut(name);
}

void addComm(const char* name, int comm)
{
    Environment::instance().addComm(name, comm);
}

bool hasComm(const char* name) {
    return Environment::instance().hasComm(name);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
