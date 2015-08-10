/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "PrintHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/Interpreter.h"

using namespace std;
using namespace eckit;

namespace eckit {

PrintHandler::PrintHandler(const string& name, const string& end, std::ostream& o)
: RequestHandler(name),
  end_(end),
  out_(&o)
{}

PrintHandler::PrintHandler(const string& name, const string& end)
: RequestHandler(name),
  end_(end),
  out_(&cout)
{}

Values PrintHandler::handle(ExecutionContext& context)
{
    // TODO: this is a leak I think
    Values r (Cell::clone(context.environment().lookupNoThrow("values")));
    if (!r )
        r = new Cell("_list", "", 0, 0);
    else
        printList(out(), r, end_);

    out() << end_;
    return r;
}

void PrintHandler::printList(ostream& out, Values values, const std::string& end)
{
    for (Cell* e(values); e; e = e->rest())
        if (e->value())
            out << e->value() << " ";
        else
            out << "(null)" << " ";
}

} // namespace eckit
