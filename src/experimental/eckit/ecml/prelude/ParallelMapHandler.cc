/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ParallelMapHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/prelude/PrintHandler.h"
#include "experimental/eckit/ecml/ast/Closure.h"
#include "experimental/eckit/ecml/core/Interpreter.h"

using namespace std;
using namespace eckit;

namespace eckit {

ParallelMapHandler::ParallelMapHandler(const std::string& name) : RequestHandler(name) {}

Values ParallelMapHandler::handle(ExecutionContext& context)
{
    List r;

    Values values (context.environment().lookup("values"));
    Values f (context.environment().lookup("f"));
    f = f->value();

    //Log::info() << "map: f = " << f << endl;
    //Log::info() << "map: values = " << values << endl;

    for (Cell* l(values); l; l = l->rest())
    {
        List values;
        values.append(l->value());

        //Log::info() << "map: values = " << (Cell*) values << endl;

        Closure closure (f);

        //Log::info() << "map: closure = " << (Cell*) values << endl;

        Cell* env (closure.capturedEnvironment());
        ASSERT(env->tag() == "_list");
        env = env->value();

        //Log::info() << "env->tag(): " << env->tag() << endl;
        //ASSERT(env->tag() == "_requests"); // _list
        env = env->value();
        //Log::info() << "env->tag(): " << env->tag() << endl;
        ASSERT(env->tag() == "_verb"); // _list
       

        //Log::info() << "map: capturedEnvironment = " << env << endl;

        env->append(new Cell("", "values", values, 0));

        //Log::info() << "map: EXTENDED capturedEnvironment = " << env << endl;

        Cell* cc (closure);

        //Log::info() << "cc: " << cc << endl;

        Cell* evaluated (context.interpreter().eval(cc, context));
        //Log::info() << "evaluated: " << evaluated << endl;
    }

    return r;
}

} // namespace eckit
