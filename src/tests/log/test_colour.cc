/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/runtime/Tool.h"
#include "eckit/log/Colour.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestColour : public Tool {
public:

    TestColour(int argc,char **argv): Tool(argc,argv) {}

    virtual void run();
};

void TestColour::run()
{
  
  Log::info() << "Hello, worlds" << std::endl;

  std::cout << Colour::red << "Red" << Colour::reset << std::endl;
  std::cout << Colour::off;
  std::cout << Colour::red << "Red" << Colour::reset << std::endl;
  std::cout << Colour::on;
  std::cout << Colour::blue << "Red" << Colour::reset << std::endl;
  std::cout << Colour::red << Colour::bold << "Red" << Colour::reset << std::endl;
  std::cout << Colour::yellow << Colour::underline << "Red" << Colour::reset << std::endl;
	
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestColour mytest(argc,argv);
    mytest.start();
    return 0;
}
