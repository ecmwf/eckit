/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/distributed/Transport.h"

#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/option/CmdArgs.h"


namespace eckit::distributed {



namespace {


static eckit::Mutex *local_mutex = 0;
static std::map<std::string, TransportFactory *> *m = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, TransportFactory *>();
}


}  // (anonymous namespace)



//----------------------------------------------------------------------------------------------------------------------

Transport::Transport(const eckit::option::CmdArgs &args):
    title_("eckit-run"),
    id_("no-id") {

}

Transport::~Transport() {

}

const std::string& Transport::title() const {
    return title_;
}

const std::string& Transport::id() const {
    return id_;
}

const TransportStatistics& Transport::statistics() const {
    return statistics_;
}

//----------------------------------------------------------------------------------------------------------------------



TransportFactory::TransportFactory(const std::string &name):
    name_(name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


TransportFactory::~TransportFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);

}

Transport *TransportFactory::build( const eckit::option::CmdArgs &args) {

    pthread_once(&once, init);


    std::string name = "none";
    args.get("transport", name);

    // if (!params.get("gridType", name)) {
    //     throw eckit::SeriousBug("TransportFactory cannot get gridType");
    // }

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, TransportFactory *>::const_iterator j = m->find(name);

    if (j == m->end()) {
        eckit::Log::error() << "No TransportFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "TransportFactories are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No TransportFactory called ") + name);
    }

    return (*j).second->make(args);
}


void TransportFactory::list(std::ostream& out) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (std::map<std::string, TransportFactory *>::const_iterator j = m->begin() ; j != m->end() ; ++j) {
        out << sep << (*j).first;
        sep = ", ";
    }
}




} // namespace eckit

