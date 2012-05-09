/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ListContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef ListContent_H
#define ListContent_H

#include "eclib/Value.h"

class ListContent : public Content {

protected:

// -- Constructor 

	ListContent();
	ListContent(const vector<Value>&);
	ListContent(const Value&);

	ListContent(Stream&);

// -- Destructor

	virtual ~ListContent();

// -- Overridden methods

    // -- From Content

	virtual void value(vector<Value>&) const;
	virtual void value(long long& n) const;
	virtual void value(string& n)    const;
	virtual void value(Date& n)      const;
	virtual void value(Time& n)      const;
	virtual void value(DateTime& n)  const;

	virtual int  compare(const Content&)         const;

	virtual int  compareNil(const NilContent&) const       { return  1; }
	virtual int  compareNumber(const NumberContent&) const { return  1; }
	virtual int  compareString(const StringContent&) const { return  1; }
	virtual int  compareList(const ListContent&) const;
	virtual int  compareTime(const TimeContent&)         const { return  -1; }
	virtual int  compareDate(const DateContent&)         const { return  -1; }
	virtual int  compareDateTime(const DateTimeContent&) const { return  -1; }

	virtual Content* add(const Content&)         const;
	virtual Content* sub(const Content&) const;
	virtual Content* mul(const Content&) const;
	virtual Content* div(const Content&) const;

	virtual Content* addList(const ListContent&) const;

	virtual void   print(ostream&) const;
	virtual string typeName() const       { return "List"; }

	virtual bool   isList() const         { return true; }

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()         { return classSpec_;}

private:

	ListContent(const ListContent&);
	ListContent& operator=(const ListContent&);

// -- Members

	vector<Value> values_;

// -- Class Members

    static  ClassSpec                  classSpec_;
    static  Reanimator<ListContent>  reanimator_;

// -- Friends

    friend class Reanimator<ListContent>;
	friend class Value;

};

#endif
