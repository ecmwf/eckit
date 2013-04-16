/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_config_Condition_H
#define eckit_config_Condition_H

#include "eckit/machine.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/types/Types.h"

#include "eckit/config/Compiler.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Condition : private NonCopyable {

public: // methods
    
    Condition( Compiler& c );
    
    ~Condition();
    
    bool eval( const StringDict& in, StringDict& out );

    void print( std::ostream& out );

protected: // method
    
    void parseSentence(Compiler& comp);
    
private: // members
    
    std::vector< std::pair<std::string,StringList> > sentences_;

};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit


#endif
