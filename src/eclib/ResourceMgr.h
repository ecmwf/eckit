/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResourceMgr.h
// Baudouin Raoult - ECMWF May 96

#ifndef ResourceMgr_H
#define ResourceMgr_H

#include "eclib/NonCopyable.h"
#include "eclib/PathName.h"
#include "eclib/Types.h"

class LocalPathName;
class Configurable;

class ResourceMgr : private NonCopyable {

public: // methods

	static void reset();
    static bool lookUp( Configurable*, const string&, const StringDict* args, string&);
	static void set(const string&,const string&);

    static void appendConfig( istream& in );
    static void appendConfig( const PathName& );
    
protected: // methods
    
    static void readConfigFiles();
    
private: // methods

	ResourceMgr();

private: // members

	static bool inited_;

};

#endif
