/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/AIOHandle.h"
#include "eckit/io/Buffer.h"
#include "eckit/filesystem/FileHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestAIOHandle : public Tool {
public:

    TestAIOHandle(int argc,char **argv): Tool(argc,argv) {}

    ~TestAIOHandle() {}

    virtual void run();
    
    void setup();
    void teardown();

    void test_write();
    void test_append();
    
    PathName path_;
    
};

//-----------------------------------------------------------------------------
            
void TestAIOHandle::test_write()
{
    DataHandle* aioh = new AIOHandle(path_);
    
    aioh->openForWrite(0);
    
    const char buf [] = "74e1feb8d0b1d328cbea63832c2dcfb2b4fa1adf";
    
    aioh->write(buf,sizeof(buf));
    
    aioh->close();
    
    delete aioh;
    
    DataHandle* fh = path_.fileHandle();
    
    fh->openForRead();
    
    Buffer buf2(1024);
    
    fh->read(buf2,buf2.size());
    fh->close();
    
    delete fh;
    
    ASSERT( buf == string(buf2) );
}

//-----------------------------------------------------------------------------
            
void TestAIOHandle::test_append()
{
    DataHandle* aioh = new AIOHandle(path_);
    
    aioh->openForAppend(0);
    
    const char buf [] = "53d50e63a50fba73f0151028a695a238ff06491c";
    
    aioh->write(buf,sizeof(buf));
    
    aioh->close();
    
    delete aioh;
    
    DataHandle* fh = path_.fileHandle();
    
    fh->openForRead();
    
    fh->seek( sizeof(buf) );
    
    Buffer buf2(1024);
    
    fh->read(buf2,buf2.size());
    fh->close();
    
    delete fh;
    
    ASSERT( buf == string(buf2) );
}

//-----------------------------------------------------------------------------

void TestAIOHandle::setup()
{
    path_ = PathName::unique( PathName( "lolo" ) );
    path_ += ".dat";
}

void TestAIOHandle::teardown()
{
    path_.unlink();    
}

//-----------------------------------------------------------------------------
            
void TestAIOHandle::run()
{
    setup();
    
    test_write();
    test_append();
    
    teardown();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestAIOHandle mytest(argc,argv);
    mytest.start();
    return 0;
}

