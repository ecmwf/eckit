/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Environment.h
// Piotr Kuchta - (c) ECMWF May 2015

#ifndef Environment_H
#define Environment_H

#include "eckit/memory/NonCopyable.h"

class RequestHandler;

class Environment : private eckit::NonCopyable {
public:
	Environment(Environment*);
	~Environment(); 

    void set(const std::string&, RequestHandler*);
    RequestHandler* lookup(const std::string&);

    Environment* parent();

protected:
	void print(std::ostream&) const; 	

private:
    std::map<std::string,RequestHandler*> dictionary_;
    Environment* parent_;
};

#endif
