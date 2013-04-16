/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/runtime/Tool.h"
#include "eclib/BadValue.h"
#include "eckit/runtime/Context.h"
#include "eclib/DataHandle.h"
#include "eclib/Log.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"
#include "eclib/Tokenizer.h"
#include "eclib/Types.h"

#include "eckit/config/Script.h"
#include "eckit/config/Compiler.h"

using namespace eckit;

class ReadConfig : public Tool {
public:

    ReadConfig(int argc,char **argv): Tool(argc,argv) {}

    ~ReadConfig() {}

    virtual void run();
};

//-----------------------------------------------------------------------------

void ReadConfig::run()
{
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
            
    // get file
    
    string filename = Resource<string>("-file","");    
    
    if( filename.empty() )
    {
        std::cerr << "empty config filename. use -file to specify filename" << std::endl;
        return;
    }
    
    PathName filepath(filename);
    
    if( ! filepath.exists() )
        throw BadValue( "file does not exist -- " + filename );
    
    config::Script s;
    
    s.readFile(filepath);

    // print ???
    
    bool print = Resource<bool>("-print",false);    
   
    if(print)
        s.print( std::cout );

    // evaluate the configuration
    
    StringDict dout;
    
    s.execute(din,dout);
    
//    std::cout << "in :" << din << std::endl;    
//    std::cout << "out:" << dout << std::endl;  

    for( StringDict::const_iterator i = dout.begin(); i != dout.end(); ++i )
        std::cout << i->first << " : " << i->second << std::endl;    
    
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    ReadConfig reader(argc,argv);
    reader.start();
    return 0;
}

