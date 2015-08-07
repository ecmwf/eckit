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

#ifndef eckit_utils_ExecutionContext_H
#define eckit_utils_ExecutionContext_H

#include "experimental/eckit/ecml/parser/Request.h"

namespace eckit {

class Environment;
class Module;
class Interpreter;
class RequestHandler;

class ExecutionContext {
public:
    ExecutionContext();
    ~ExecutionContext();

    void registerHandler(const char*, eckit::RequestHandler&);
    void registerHandler(const std::string&, eckit::RequestHandler&);

    void execute(const std::string&);
    void executeScriptFile(const std::string&);

    void import(eckit::Module&);

    /// Creates a new environment frame and pushes it onto stack.
    void pushEnvironmentFrame();

    void pushEnvironmentFrame(eckit::Request);
    void popEnvironmentFrame();
    void popEnvironmentFrame(eckit::Request);

    Environment& environment();

    void interpreter(eckit::Interpreter*);
    eckit::Interpreter& interpreter() const;

    Cell* copy() const;

private:
    eckit::Environment* environment_;
    eckit::Interpreter* interpreter_;
};

} // namespace eckit

#endif
