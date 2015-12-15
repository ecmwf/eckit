/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/types/Types.h"
//#include "eckit/filesystem/FileSpace.h"
#include "eckit/io/FileHandle.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/Interpreter.h"


#include "RunHandler.h"

using namespace std;

namespace eckit {


RunHandler::RunHandler(const std::string& name) : SpecialFormHandler(name) {}

Values RunHandler::handle(const Request request, ExecutionContext& context)
{
    Cell* r(0); 

    Request evaluatedAttributes (context.interpreter().evalAttributes(request, context));

    std::vector<std::string> source;
    Values vs (evaluatedAttributes->valueOrDefault("source", 0));
    if (vs)
        for (Cell* p (vs); p; p = p->rest())
        {
            Cell* v (p->value());
            source.push_back(v->str());
        }

    Log::info() << "run: " << source << endl;

    Request frame (new Cell("_verb", "let", 0, 0));
    context.pushEnvironmentFrame(frame);

    for (size_t i(0); i < source.size(); ++i)
    {
        PathName fileName (source[i]);

        Log::info() << "* Run " << fileName << endl;

        delete r;
        r = context.executeScriptFile(fileName);
    }

    List rv;
    rv.append(r);

    return rv;
}

} // namespace eckit
