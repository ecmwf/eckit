/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "experimental/eckit/ecml/parser/Request.h"

#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/Interpreter.h"

#include "experimental/eckit/ecml/prelude/ForHandler.h"

using namespace std;

namespace eckit {

ForHandler::ForHandler(const string& name)
: SpecialFormHandler(name)
{}

// for,x = 1/2/3,
//     do = (println,_=($,_=x))
// =>
//   let,x=1
//   println,_=($,_=x) 
//
//   let,x=2
//   println,_=($,_=x) 
//
//   ...

// for, x = 1/2/3,
//      do = (for, y = a/b/c,
//                 do = (println,_ = ($,_=x) / " " / ($,_=y)))

Request ForHandler::handle(const Request r, ExecutionContext& context)
{
    Cell* loopVariable (r->rest());

    if (! loopVariable)
        throw UserError("for: first parameter must be name and values of loop variable");

    string var (loopVariable->text());
    Cell* values (context.interpreter().eval(loopVariable->value(), context));

    Cell* loopBody (loopVariable->rest());

    if (! loopBody || StringTools::lower(loopBody->text()) != "do" )
        throw UserError("for: second parameter must be 'do' (body of the loop)");

    Cell* loopBodyCode (loopBody->value()->value());
    ASSERT(loopBodyCode->tag() == "_requests");

    vector<Cell*> vvalues;
    vector<Cell*> vresult;

    for (Cell* v (values); v; v = v->rest())
    {
        Cell* value (Cell::clone(v->value()));
        vvalues.push_back(value);
        vresult.push_back(0);
    }

    #pragma omp parallel
    {
        #pragma omp for
        for (size_t i(0); i < vvalues.size(); ++i)
        {
            ExecutionContext ctx (context);

            Request frame (new Cell("_verb", "let", 0, 0));
            frame->append(new Cell("", var, Cell::clone(vvalues[i]), 0));
            ctx.pushEnvironmentFrame(frame);

            vresult[i] = ctx.interpreter().evalRequests(loopBodyCode, ctx);
        }
    }

    List result;
    for (size_t i(0); i < vresult.size(); ++i)
        result.append(vresult[i]);
    return result;
}

} // namespace eckit
