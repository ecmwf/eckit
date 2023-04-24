/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "grit/Figure.h"

#include <limits>
#include <map>
#include <ostream>

#include "grit/exception.h"
#include "grit/log.h"
#include "grit/util/Mutex.h"


namespace grit {


static util::once_flag __once;
static util::recursive_mutex* __mutex                                 = nullptr;
static std::map<FigureFactory::key_type, FigureFactory*>* __factories = nullptr;


static void __init() {
    __mutex     = new util::recursive_mutex();
    __factories = new std::map<FigureFactory::key_type, FigureFactory*>();
}


FigureFactory::FigureFactory(const FigureFactory::key_type& key) : key_(key) {
    util::call_once(__once, __init);
    util::lock_guard<util::recursive_mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        throw exception::runtime_error("FigureFactory: duplicate '" + key + "'");
    }

    (*__factories)[key] = this;
}


FigureFactory::~FigureFactory() {
    util::lock_guard<util::recursive_mutex> lock(*__mutex);

    if (__factories != nullptr) {
        __factories->erase(key_);
    }
}


Figure* FigureFactory::build(const FigureFactory::key_type& key) {
    util::call_once(__once, __init);
    util::lock_guard<util::recursive_mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        return f->second->make();
    }

    list(error << "FigureFactory: unknown '" << key << "', choices are: ");
    throw exception::runtime_error("FigureFactory: unknown '" + key + "'");
}


void FigureFactory::list(std::ostream& out) {
    util::call_once(__once, __init);
    util::lock_guard<util::recursive_mutex> lock(*__mutex);

    const char* sep = "";
    for (const auto& j : *__factories) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace grit
