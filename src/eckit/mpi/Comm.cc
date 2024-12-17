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

#include <algorithm>
#include <map>

#include "eckit/eckit_config.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit::mpi {

constexpr bool have_parallel() {
#if eckit_HAVE_MPI
    return true;
#else
    return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------

class Environment {
public:
    static std::string_view getDefaultCommType() {
        // Force a given communicator (only required if e.g. running serial applications with MPI)
        if (const char* forcedComm = ::getenv("ECKIT_MPI_FORCE")) {
            return forcedComm;
            // Use parallel communicator if in an MPI environment
        }
        if (have_parallel()) {
            const std::string defaultMPIDetectionVars =
                "OMPI_COMM_WORLD_SIZE"    // OpenMPI
                ",ALPS_APP_PE"            // Cray aprun
                ",PMI_SIZE"               // Intel MPI
                ",SLURM_STEP_NUM_TASKS";  // slurm srun
            std::string eckitMPIDetectionVars = eckit::LibResource<std::string, LibEcKit>("$ECKIT_MPI_DETECTION_VARS;eckitMPIDetectionVars", defaultMPIDetectionVars);
            std::vector<std::string> envVars;
            Tokenizer{','}(eckitMPIDetectionVars, envVars);
            for (const auto& env : envVars) {
                if (::getenv(env.c_str())) {
                    return "parallel";
                }
            }
        }
        return "serial";
    }

    static Environment& instance() {
        static Environment env;
        return env;
    }

    void initDefault() {
        AutoLock<Mutex> lock(mutex_);

        ASSERT(!default_);

        Comm* world = CommFactory::build("world", getDefaultCommType());

        communicators[world->name()] = world;

        Comm* self                  = world->self();
        communicators[self->name()] = self;

        default_ = world;
    }

    void setDefault(std::string_view name) {
        AutoLock<Mutex> lock(mutex_);

        auto itr = communicators.find(name);
        if (itr != communicators.end()) {
            default_ = itr->second;
            return;
        }

        eckit::Log::error() << "Cannot set default communicator to '" << name
                            << "', no communicator with that name was found" << std::endl;
        eckit::Log::error() << "Current communicators are:" << std::endl;
        for (itr = communicators.begin(); itr != communicators.end(); ++itr) {
            eckit::Log::error() << "   " << (*itr).first << std::endl;
        }
        throw eckit::SeriousBug("No communicator called " + std::string{name}, Here());
    }

    bool hasComm(std::string_view name) {
        AutoLock<Mutex> lock(mutex_);
        std::map<std::string, Comm*>::iterator itr = communicators.find(name);
        if (itr != communicators.end()) {
            return true;
        }
        return false;
    }

    std::vector<std::string> listComms() {
        AutoLock<Mutex> lock(mutex_);
        std::vector<std::string> allComms;

        std::transform(begin(communicators), end(communicators), std::back_inserter(allComms),
                       [](const std::pair<std::string, Comm*>& c) { return c.first; });

        return allComms;
    }

    void finaliseAllComms() {
        AutoLock<Mutex> lock(mutex_);

        auto itr = communicators.begin();
        for (; itr != communicators.end(); ++itr) {
            delete itr->second;
        }
        communicators.clear();
    }

    Comm& getComm(std::string_view name = {}) {
        AutoLock<Mutex> lock(mutex_);

        if (name.empty() && default_) {
            return *default_; /* most common case first */
        }

        if (!default_) {
            initDefault();
        }

        if (name.empty()) {
            ASSERT(default_); /* sanity check init was successful */
            return *default_;
        }

        auto itr = communicators.find(name);
        if (itr != communicators.end()) {
            return *itr->second;
        }

        eckit::Log::error() << "No Communicator '" << name << "'" << std::endl;
        eckit::Log::error() << "Current communicators are:" << std::endl;
        for (itr = communicators.begin(); itr != communicators.end(); ++itr) {
            eckit::Log::error() << "   " << (*itr).first << std::endl;
        }
        throw eckit::SeriousBug("No communicator called " + std::string{name}, Here());
    }

    void addComm(std::string_view name, int comm) {
        AutoLock<Mutex> lock(mutex_);

        if (hasComm(name)) {
            throw SeriousBug("Communicator with name " + std::string{name} + " already exists", Here());
        }

        Comm* pComm         = CommFactory::build(name, getDefaultCommType(), comm);
        communicators.emplace(name, pComm);
    }

    void addComm(std::string_view name, Comm* comm) {
        AutoLock<Mutex> lock(mutex_);

        if (hasComm(name)) {
            throw SeriousBug("Communicator with name " + std::string{name} + " already exists", Here());
        }
        communicators.emplace(name, comm);
    }

    void deleteComm(std::string_view name) {
        AutoLock<Mutex> lock(mutex_);

        auto itr = communicators.find(name);

        if (itr != communicators.end()) {

            Comm* comm = itr->second;

            // refuse to delete the default communicator
            if (default_ == comm) {
                throw SeriousBug("Trying to delete the default Communicator with name "
                                     + std::string{name},
                                 Here());
            }

            comm->free();
            delete comm;

            communicators.erase(itr);
        }
        else {
            throw SeriousBug("Communicator with name " + std::string{name} + " does not exist", Here());
        }
    }


    Environment() :
        default_(nullptr) {}

    ~Environment() {
        AutoLock<Mutex> lock(mutex_);

        finaliseAllComms();
        default_ = nullptr;
    }

    Comm* default_;

    std::map<std::string, Comm*, std::less<>> communicators;

    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

class CommFactories {
public:
    void registFactory(std::string_view builder, CommFactory* f) {
        AutoLock<Mutex> lock(mutex_);
        ASSERT(factories.find(builder) == factories.end());
        factories.emplace(builder, f);
    }

    void unregistFactory(std::string_view builder) {
        AutoLock<Mutex> lock(mutex_);
        factories.erase(std::string{builder});
    }

    CommFactory& getFactory(std::string_view builder) const {
        AutoLock<Mutex> lock(mutex_);

        auto j = factories.find(builder);

        if (j != factories.end()) {
            return *(j->second);
        }

        eckit::Log::error() << "No CommFactory for [" << builder << "]" << std::endl;
        eckit::Log::error() << "CommFactories are:" << std::endl;
        for (j = factories.begin(); j != factories.end(); ++j) {
            eckit::Log::error() << "   " << (*j).first << std::endl;
        }

        throw eckit::SeriousBug("No CommFactory called " + std::string{builder}, Here());
    }

    static CommFactories& instance() {
        static CommFactories obj;
        return obj;
    }

private:
    CommFactories() {}

    std::map<std::string, CommFactory*, std::less<>> factories;
    mutable eckit::Mutex mutex_;
};

CommFactory::CommFactory(std::string_view builder) :
    builder_(builder) {
    CommFactories::instance().registFactory(builder, this);
}

CommFactory::~CommFactory() {
    CommFactories::instance().unregistFactory(builder_);
}

Comm* CommFactory::build(std::string_view name, std::string_view builder) {
    return CommFactories::instance().getFactory(builder).make(name);
}

Comm* CommFactory::build(std::string_view name, std::string_view builder, int comm) {
    return CommFactories::instance().getFactory(builder).make(name, comm);
}

//----------------------------------------------------------------------------------------------------------------------

Comm::Comm(std::string_view name) :
    name_(name) {}

Comm::~Comm() {}

//----------------------------------------------------------------------------------------------------------------------

Comm& comm(std::string_view name) {
    return Environment::instance().getComm(name);
}

void setCommDefault(std::string_view name) {
    Environment::instance().setDefault(name);
}

void addComm(std::string_view name, int comm) {
    Environment::instance().addComm(name, comm);
}
void addComm(std::string_view name, Comm* comm) {
    Environment::instance().addComm(name, comm);
}

void deleteComm(std::string_view name) {
    Environment::instance().deleteComm(name);
}

bool hasComm(std::string_view name) {
    return Environment::instance().hasComm(name);
}

std::vector<std::string> listComms() {
    return Environment::instance().listComms();
}

void finaliseAllComms() {
    return Environment::instance().finaliseAllComms();
}

Comm& self() {
    return comm("self");
}

namespace detail {

void Assert(int code, const char* msg, const char* file, int line, const char* func) {
    ::eckit::Assert(code, msg, file, line, func);
}

}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
