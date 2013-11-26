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

#include "eckit/maths/IfElse.h"
#include "eckit/maths/Lambda.h"
#include "eckit/maths/ParamDef.h"
#include "eckit/maths/Call.h"
#include "eckit/maths/Bind.h"

#include "eckit/maths/Math.h"

using namespace std;

using namespace eckit;
using namespace eckit::maths;

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

class Currying : public Tool {
public:

    Currying(int argc,char **argv): Tool(argc,argv) {
    }

    ~Currying() {
    }

    virtual void run();
};

//-----------------------------------------------------------------------------

void Currying::run()
{

    setformat(cout,maths::CodeFormat);
    {
        Math X = maths::lambda("i", "j", Math("i") + Math("j"));
        Math Y = maths::call(X);

        std::cout << "-----------------------" << std::endl;
        std::cout << Y << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << Y(1.0, 2.3) << std::endl;
    }


    {
        Math X = maths::lambda("i", maths::call(maths::lambda("j", Math("i") + Math("j"))));
        Math Y = maths::call(X, Math(1.0));

        std::cout << "-----------------------" << std::endl;
        std::cout << Y << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << Y(2.3) << std::endl;
    }

    {
        Math X = maths::lambda("i", "j", Math("i") + Math("j"));
        Math B = maths::bind<2>(call(X), Math(3.9));
        cout << B << endl;
        cout << B(2.9) << endl;
    }

    Math m = maths::call(maths::lambda("i", maths::call(maths::lambda("j", (Math("i") + Math("j"))))), Math(1.0));
    std::cout << m << std::endl;
    std::cout << m(9.0) << std::endl;
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    Currying app(argc,argv);
    app.start();
    return 0;
}

