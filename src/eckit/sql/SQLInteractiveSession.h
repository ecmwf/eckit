/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLInteractiveSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLInteractiveSession_H
#define SQLInteractiveSession_H

#include "SQLSession.h"

namespace eckit {
namespace sql {

class SQLInteractiveSession : public SQLSession {
public:
    SQLInteractiveSession(std::ostream & = std::cout);
	~SQLInteractiveSession(); 

    void interactive();

private:
// No copy allowed
	SQLInteractiveSession(const SQLInteractiveSession&);
	SQLInteractiveSession& operator=(const SQLInteractiveSession&);

    std::ostream &out_;
    SQLStatement* sql_;

// -- Overridden methods
    virtual void statement(SQLStatement*);
    virtual SQLStatement* statement();
    virtual SQLOutput* defaultOutput();

	//friend std::ostream& operator<<(std::ostream& s,const SQLInteractiveSession& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace eckit 

#endif
