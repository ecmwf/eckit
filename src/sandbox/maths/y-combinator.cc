/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/maths/BinaryFunc.h"
#include "eckit/maths/BinaryPredicate.h"
#include "eckit/maths/Count.h"
#include "eckit/maths/Expr.h"
#include "eckit/maths/FMap.h"
#include "eckit/maths/Filter.h"
#include "eckit/maths/Reduce.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/UnaryFunc.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/ZipWith.h"
#include "eckit/maths/Bind.h"
#include "eckit/maths/IfElse.h"
#include "eckit/maths/Function.h"
#include "eckit/maths/Param.h"
#include "eckit/maths/ParamDef.h"

using namespace eckit;
using namespace eckit::maths;

using namespace eckit;

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


//-----------------------------------------------------------------------------

void YCombinator::run()
{
    ExpPtr a = maths::scalar( 2. );
    ExpPtr b = maths::scalar( 4. );
    ExpPtr c = maths::scalar( 7. );
    ExpPtr z = maths::scalar( 0. );

#if 0
    // Here's the function that we want to recurse.
    X = function (recurse, n) {
      if (0 == n)
        return 1;
      else
        return n * recurse(recurse, n - 1);
    };

    // This will get X to recurse.
    Y = function (builder, n) {
      return builder(builder, n);
    };

    // Here it is in action.
    Y(
      X,
      5
    );
#endif


    ExpPtr g = maths::greater(a, z);
    cout << g->str() << endl;

    cout << g->eval(a, z)->str() << endl;

     ExpPtr e = maths::function(
                 maths::paramdef("1"), maths::paramdef("2"),
                 ifelse(maths::greater(maths::parameter("1"), maths::parameter("2")), b, c));

     cout << e->str() << endl;

     cout << e->eval(a, z)->str() << endl;
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    YCombinator app(argc,argv);
    app.start();
    return 0;
}

