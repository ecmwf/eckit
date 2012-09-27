/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Application.h"
#include "eclib/Buffer.h"
#include "eclib/Context.h"
#include "eclib/DataHandle.h"
#include "eclib/Log.h"
#include "eclib/PathName.h"
#include "eclib/Resource.h"
#include "eclib/Tokenizer.h"
#include "eclib/Types.h"

#include "eclib/config/Script.h"
#include "eclib/config/Compiler.h"

class ReadConfig : public Application {
public:

    ReadConfig(int argc,char **argv): Application(argc,argv) {}

    ~ReadConfig() {}

    virtual void run();
};

//-----------------------------------------------------------------------------

void ReadConfig::run()
{
    // get file
    
    string filename = Resource<string>("-file","");    
    
    if( filename.empty() )
    {
        Log::error() << "empty config filename. use -file to specify filename" << std::endl;
        return;
    }
    
    PathName filepath(filename);
    
    DataHandle* file ( filepath.fileHandle() );
    file->openForRead();
    AutoClose close(*file);

    // get values
    
    string valueStr  = Resource<string>("-values","");
    StringList values;
    
    Tokenizer parse(":,");
    parse( valueStr, values );
    
    if( values.size() % 2 != 0 )
    {
        Log::error() << "missing value -- values must be supplied in colon or comma separated key:value pairs" << std::endl;
        return;
    }
    
    // convert values to dictionary
    
    StringDict din;
    for( size_t i = 0; i < values.size(); ++i, ++i )
        din[ values[i] ] = values[i+1];
            
    // read the file
    
    size_t length  = file->estimate();
    ASSERT( length );
    
    Buffer buffer( length );
    
    file->read(buffer,length);
    
    // compile the file contents    
    
    const char * b = buffer;
    
    std::cout << b << std::endl;
    
    istringstream in( b );
    
    config::Compiler c(in);
    
    config::Script s(c);
    
//    s.print( std::cout );
    
    StringDict dout;
    
    s.execute(din,dout);
    
    std::cout << "out:" << dout << std::endl;    
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    ReadConfig reader(argc,argv);
    reader.start();
    return 0;
}

