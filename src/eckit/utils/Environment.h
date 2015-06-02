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

#include "eckit/parser/Request.h"

class ExecutionContext;

class Environment {
public:
	Environment(Environment*, Request);
	~Environment(); 

    void set(const std::string&, Request);
    Request lookup(const std::string&);
    Request lookupNoThrow(const std::string&);
    std::string lookup(const std::string&, const std::string&, ExecutionContext&);
    std::vector<std::string> lookupList(const std::string&, ExecutionContext&);

    Environment* parent();
    Request currentFrame();

protected:
	void print(std::ostream&) const; 	

private:
    Cell* dictionary_;
    Environment* parent_;
};

#endif
