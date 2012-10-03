/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Log.h"
#include "eclib/Tool.h"
#include "eclib/Tokenizer.h"
#include "eclib/Types.h"

class TestTokenizer : public Tool {
public:

    TestTokenizer(int argc,char **argv): Tool(argc,argv) {}

    ~TestTokenizer() {}

    virtual void run();

    void test_single();
    void test_multi();
};

//-----------------------------------------------------------------------------

void TestTokenizer::test_single()
{
    std::string source (":lolo:lolo:lolo");
    StringList  target;

    Tokenizer parse(":");

    parse(source,target);

    ASSERT( target.size() == 3 );

    for( size_t i = 0; i < target.size(); ++i )
        ASSERT( target[i] == "lolo" );
}

//-----------------------------------------------------------------------------

void TestTokenizer::test_multi()
{
    std::string source ("-lolo-lolo-lolo;lolo:lolo-");
    StringList  target;

    Tokenizer parse("-:;");

    parse(source,target);

    ASSERT( target.size() == 5 );

    for( size_t i = 0; i < target.size(); ++i )
        ASSERT( target[i] == "lolo" );}

//-----------------------------------------------------------------------------
            
void TestTokenizer::run()
{
    test_single();
    test_multi();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestTokenizer mytest(argc,argv);
    mytest.start();
    return 0;
}

