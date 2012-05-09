/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NilContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef NilContent_H
#define NilContent_H

#include "eclib/Content.h"

class NilContent : public Content {
protected:

// -- Constructors

	NilContent();
	NilContent(Stream&);

// -- Destructors

	virtual ~NilContent();

// -- Overridden Methods

	// From Content

	virtual void value(long long& n)     const { Content::value(n); }
	virtual void value(string& n)        const { Content::value(n); }
	virtual void value(vector<Value>& n) const { }
	virtual void value(Date& n)      const { Content::value(n); }
	virtual void value(Time& n)      const { Content::value(n); }
	virtual void value(DateTime& n)  const { Content::value(n); }

	virtual int  compare(const Content&)       const;
	virtual int  compareNil(const NilContent&) const;
	virtual int  compareNumber(const NumberContent&) const { return  -1; }
	virtual int  compareString(const StringContent&) const { return  -1; }
	virtual int  compareList(const ListContent&)     const { return  -1; }
	virtual int  compareTime(const TimeContent&)         const { return  -1; }
	virtual int  compareDate(const DateContent&)         const { return  -1; }
	virtual int  compareDateTime(const DateTimeContent&) const { return  -1; }

	virtual Content* add(const Content&)        const;
	virtual Content* sub(const Content&) const;
	virtual Content* mul(const Content&) const;
	virtual Content* div(const Content&) const;

	virtual Content* addNil(const NilContent&)  const;
	virtual Content* subNil(const NilContent&)  const;
	virtual Content* mulNil(const NilContent&)  const;
	virtual Content* divNil(const NilContent&)  const;

	virtual bool     isNil()     const  { return true; }
	virtual string   typeName()  const  { return "Nil"; }
	virtual void     print(ostream&) const;

	// From Streamable

    virtual void     encode(Stream&)              const;
    virtual const    ReanimatorBase& reanimator() const { return reanimator_; }


private:

// -- No copy allowed

	NilContent(const NilContent&);
	NilContent& operator=(const NilContent&);

// -- Class Members

    static  ClassSpec               classSpec_;
    static  Reanimator<NilContent>  reanimator_;

// -- Friends

	friend class Reanimator<NilContent>;
	friend class Value;
};

#endif
