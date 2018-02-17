/*
 * (C) Copyright 1996- ECMWF.
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

    static const char* getDefaultComm() {

        // Force a given communicator (only required if e.g. running serial applications with MPI)
        if (const char* forcedComm = ::getenv("ECKIT_MPI_FORCE")) {
            return forcedComm;
        // Use parallel communicator if in an MPI environment
        } else if (::getenv("OMPI_COMM_WORLD_SIZE") || // OpenMPI
                   ::getenv("ALPS_APP_PE")          || // Cray PE
                   ::getenv("PMI_SIZE")) {             // Intel
            return "parallel";
        // Use serial communicator otherwise
        } else {
            return "serial";
        }
    }

    static Environment& instance() {
        static Environment env;
        return env;
    }

    void initDefault() {

        AutoLock<Mutex> lock(mutex_);

        ASSERT(!default_);

        Comm* world = CommFactory::build(getDefaultComm());

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

    void finaliseAllComms() {
        std::map<std::string, Comm*>::iterator itr = communicators.begin();
        for(; itr != communicators.end(); ++itr) {
            delete itr->second;
        }
        communicators.clear();
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

        Comm* pComm = CommFactory::build(getDefaultComm(), comm);
        communicators[name] = pComm;
    }

    Environment() : default_(0) {}

    ~Environment() {

        AutoLock<Mutex> lock(mutex_);

        finaliseAllComms();
        default_ = 0;
    }

    Comm* default_;

    std::map<std::string, Comm*> communicators;

    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

class CommFactories {
public:

  void registFactory(const std::string& name, CommFactory* f) {
      AutoLock<Mutex> lock(mutex_);
      ASSERT(factories.find(name) == factories.end());
      factories[name] = f;
  }

  void unregistFactory(const std::string& name) {
      AutoLock<Mutex> lock(mutex_);
      factories.erase(name);
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

  static CommFactories& instance() {
    static CommFactories obj;
    return obj;
  }

private:

  CommFactories() {}

  std::map<std::string, CommFactory*> factories;
  eckit::Mutex mutex_;
};

CommFactory::CommFactory(const std::string &name):
    name_(name) {
    CommFactories::instance().registFactory(name, this);
}

CommFactory::~CommFactory() {
    CommFactories::instance().unregistFactory(name_);
}

Comm* CommFactory::build(const std::string& name) {
    return CommFactories::instance().getFactory(name).make();
}

Comm*CommFactory::build(const std::string& name, int comm)
{
    return CommFactories::instance().getFactory(name).make(comm);
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

void finaliseAllComms() {
    return Environment::instance().finaliseAllComms();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace mpi
} // namepsace eckit
