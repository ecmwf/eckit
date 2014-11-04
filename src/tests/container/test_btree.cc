/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Tool.h"
#include "eckit/container/BTree.h"
#include "eckit/os/Semaphore.h"
#include "eckit/types/FixedString.h"

using namespace std;
using namespace eckit;

class Test : public Tool {
    virtual void run();

    public:

    Test(int argc, char** argv): Tool(argc,argv) { }

};


void Test::run()
{
    char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\0";

    int count = strlen(test);

    for(int j = 0; j < 100 ; j++)
    {
        std::random_shuffle(test,test+count);

        unlink("foo");
        BTree<char,int, 128> b("foo");

        for(int i = 0; i < count;  i++)
        {
            b.set(test[i],-int(i));
        }

        std::cout << std::endl;
        b.dump();
        std::cout << std::endl;

        for(int i = 0; i < count; i++)
        {
            int k;
            ASSERT(b.get(test[i],k) && k == (-int(i)));
        }
    }

    {
        unlink("bar");

        BTree<FixedString<80>,int,1024> b("bar");
        string f("foo");

        b.set(f, 22);
        b.set("barbar", 44);
        b.set("zoulou", 484);
        b.set("ZOULOU", 484);

        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
            b.set(s,-int(i));
        }


        b.dump();
    }


    if(1)
    {

        unlink("bar");

        BTree< FixedString<80>, FixedString<256>, 2048 > b("bar");
        string f("foo");


        for(int i = 0; i < count;  i++)
        {
            string s;
            s += test[i];
            b.set(s,s);
        }


        b.set(f,f);

        FixedString<256> z;
        b.get(f, z);
        std::cout << string(z) << std::endl;

        b.dump();

    }

}

//=============================================================

int main(int argc,char **argv)
{
    Test app(argc,argv);
    app.start();
    return 0;
}
