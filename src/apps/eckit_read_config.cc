/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Compiler.h"
#include "eckit/config/Resource.h"
#include "eckit/config/Script.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/Log.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/ContextBehavior.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

class ReadConfig : public Tool {
public:

    ReadConfig(int argc,char **argv): Tool(argc,argv) {}

    ~ReadConfig() {}

    virtual void run();
};

void ReadConfig::run()
{
    // get values
    
    std::string valueStr  = Resource<std::string>("-values","");
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
    
    std::string filename = Resource<std::string>("-file","");
    
    if( filename.empty() )
    {
        Log::error() << "missing config filename. use -file to specify filename" << std::endl;
        return;
    }
    
    PathName filepath(filename);
    
    if( ! filepath.exists() )
        throw BadValue( "file does not exist -- " + filename );
    
    config::Script s;

    std::stringstream stream;

    FileReadPolicy p = Context::instance().behavior().fileReadPolicy();

    read(p,filepath,stream);
    
    s.readStream(stream);

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

} // namespace eckit

//-----------------------------------------------------------------------------

using namespace eckit;

int main(int argc,char **argv)
{
    ReadConfig app(argc,argv);
    return app.start();
}

