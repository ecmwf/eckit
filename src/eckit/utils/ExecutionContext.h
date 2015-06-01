/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef ExecutionContext_H
#define ExecutionContext_H

#include "Stack.h"

class Environment;
class Module;

class ExecutionContext {
public:
    ExecutionContext();
    ~ExecutionContext();

    void import(Module&);

    /// Creates a new environment frame and pushes it onto stack.
    void pushEnvironmentFrame();

    void pushEnvironmentFrame(Request);
    void popEnvironmentFrame();
    void popEnvironmentFrame(Request);

    Environment& environment();

private:
    Environment* environment_;
};

#endif
