/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/Stack.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

Stack::Stack() {}

Stack::~Stack() { while(size()) popFrame(); }

const SortedTables::iterator& Stack::tablesIterator() 
{
	//if (size() == 0) 
	//	return end();
	return top()->tablesIterator_;
}

void Stack::pushFrame(const SortedTables::iterator ti) 
{
	push(new Environment(ti));
}

void Stack::popFrame()
{
	if (size())
	{
		delete top();
		pop();
	}
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql 
} // namespace eckit
