/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Lambda.h"
#include "eckit/maths/ParamDef.h"
#include "eckit/maths/Call.h"

#include "eckit/maths/Math.h"

using namespace std;

using namespace eckit;
using namespace eckit::maths;

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

class YCombinator : public Tool {
public:

    YCombinator(int argc,char **argv): Tool(argc,argv) {
    }

    ~YCombinator() {
    }

    virtual void run();
};

//-----------------------------------------------------------------------------

void YCombinator::run()
{

    Math Y = lambda("f", call(Math("f"), Math("f")));

    Math fact = lambda("f",
                       call(lambda("n",
                                   ifelse(
                                       Math("n") == Math(0.0),
                                       Math(1.0),
                                       Math("n") * call( Math("f"), Math("f"), Math("n") - Math(1.0))
                                       )
                                   )
                            )
                       );


    setformat(cout, maths::CodeFormat);

    Math factorial = call(Y, fact);
    std::cout << factorial << std::endl;
    setformat(cout, 0);

    std::cout << "-----------------------" << std::endl;
    std::cout << factorial(5.0) << std::endl;



    Math fib = lambda("f",
                      call(lambda("n",
                                  ifelse(
                                      Math("n") > Math(1.0),
                                      Math(call(Math("f"), Math("f"), Math("n") - Math(1.0))) +
                                      Math(call(Math("f"), Math("f"), Math("n") - Math(2.0))),
                                      Math("n")
                                      ))));

    Math fibonnaci = call(Y, fib);

    std::cout << fibonnaci(20.0) << std::endl;
}


//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    YCombinator app(argc,argv);
    app.start();
    return 0;
}

