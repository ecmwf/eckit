/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Module.h
// Piotr Kuchta - (c) ECMWF May 2015

#ifndef Module_H
#define Module_H

namespace eckit {

class ExecutionContext;

/**
 * Each software package/library X willing to add some functionality to the MARS language
 * in forms of a new (native) function aka verb, should derive a class from eckit::RequestHandler
 * and create instance of it, for example as a static local variable in <X>Module::importInto,
 * which will register the new RequestHandler, and also insert reference to them in the
 * context().environment, see ODBModule.cc in ODB API. 
 *
 * NOTE: this is a work in progress, no details how to insert reference here as it may
 * get outdated qwuickly, see ODBModule::importInto for the current way of doing it.
 * 
 * After running importInto the new verbs ("native functions") should be avialable in the
 * ExecutionContext.
 */

class Module {
public:
	virtual ~Module(); 

    virtual void importInto(ExecutionContext&) = 0;
};

} // namespace eckit

#endif
