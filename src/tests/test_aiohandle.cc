/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/AIOHandle.h"
#include "eclib/Buffer.h"
#include "eclib/FileHandle.h"
#include "eclib/PathName.h"
#include "eclib/Log.h"
#include "eclib/Tool.h"
#include "eclib/Types.h"

class TestAIOHandle : public Tool {
public:

    TestAIOHandle(int argc,char **argv): Tool(argc,argv) {}

    ~TestAIOHandle() {}

    virtual void run();
    
    void test_write();

};

//-----------------------------------------------------------------------------
            
void TestAIOHandle::test_write()
{
    PathName path ( "lolo" );
    path = PathName::unique( path );
    path += ".dat";
        
    DataHandle* aioh = new AIOHandle(path);
    
    aioh->openForWrite(0);
    
    const char buf [] = "dvnj4795gvzsdmklcneoaghwuth";
    
    aioh->write(buf,sizeof(buf));
    
//    aioh->flush();
    aioh->close();
    
    delete aioh;
    
    DataHandle* fh = path.fileHandle();
    
    fh->openForRead();
    
    Buffer buf2(1024);
    
    fh->read(buf2,buf2.size());
    fh->close();
    
    delete fh;
    
    ASSERT( buf == string(buf2) );
    
    path.unlink();
}

//-----------------------------------------------------------------------------
            
void TestAIOHandle::run()
{
    test_write();
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    TestAIOHandle mytest(argc,argv);
    mytest.start();
    return 0;
}

