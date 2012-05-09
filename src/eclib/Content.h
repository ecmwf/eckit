/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Content.h
// Manuel Fuentes - ECMWF Jun 96

#ifndef Content_H
#define Content_H

#include "eclib/Counted.h"
#include "eclib/Streamable.h"

// List here all the Content's

class NumberContent;
class StringContent;
class NilContent;
class ListContent;
class DateContent;
class TimeContent;
class DateTimeContent;
class Date;
class Time;
class DateTime;
class Value;

// Assuptions for comparisons:
// Nil < Number < String < List

// =================================================================

class Content : public Counted, public Streamable {

public:

	// Double-dispatching on subclasses
	// Needs a compare??? for every new subclass

	virtual int  compareNumber(const NumberContent&) const;
	virtual int  compareString(const StringContent&) const;
	virtual int  compareNil(const NilContent&)       const;
	virtual int  compareList(const ListContent&)     const;

	virtual int  compareDate(const DateContent&)         const;
	virtual int  compareTime(const TimeContent&)         const;
	virtual int  compareDateTime(const DateTimeContent&) const;

	// Double-dispatching on subclasses for addition
	// Needs an add??? for every new subclass

	virtual Content* addNumber(const NumberContent&)  const;
	virtual Content* addString(const StringContent&)  const;
	virtual Content* addNil(const NilContent&)        const;
	virtual Content* addList(const ListContent&)      const;
	virtual Content* addDate(const DateContent&)         const;
	virtual Content* addTime(const TimeContent&)         const;
	virtual Content* addDateTime(const DateTimeContent&) const;

	virtual Content* subNumber(const NumberContent&)  const;
	virtual Content* subString(const StringContent&)  const;
	virtual Content* subNil(const NilContent&)        const;
	virtual Content* subList(const ListContent&)      const;
	virtual Content* subDate(const DateContent&)         const;
	virtual Content* subTime(const TimeContent&)         const;
	virtual Content* subDateTime(const DateTimeContent&) const;

	virtual Content* mulNumber(const NumberContent&)  const;
	virtual Content* mulString(const StringContent&)  const;
	virtual Content* mulNil(const NilContent&)        const;
	virtual Content* mulList(const ListContent&)      const;
	virtual Content* mulDate(const DateContent&)         const;
	virtual Content* mulTime(const TimeContent&)         const;
	virtual Content* mulDateTime(const DateTimeContent&) const;

	virtual Content* divNumber(const NumberContent&)  const;
	virtual Content* divString(const StringContent&)  const;
	virtual Content* divNil(const NilContent&)        const;
	virtual Content* divList(const ListContent&)      const;
	virtual Content* divDate(const DateContent&)         const;
	virtual Content* divTime(const TimeContent&)         const;
	virtual Content* divDateTime(const DateTimeContent&) const;


protected:

// -- Constructor

	Content()          {  }
	Content(Stream&);

// -- Destructor

	virtual ~Content();

// -- Operators

	bool operator==(const Content&) const;
	bool operator<(const Content&)  const;


//	void *operator new(size_t s)  { return MemoryPool::fastAllocate(s);}
//	void operator delete(void* p) { MemoryPool::fastDeallocate(p);     }

// -- Methods

	virtual void value(long long&) const;
	virtual void value(string&)    const;
	virtual void value(Date&)      const;
	virtual void value(Time&)      const;
	virtual void value(DateTime&)  const;
	virtual void value(vector<Value>&) const;

	Content* operator+(const Content&) const;
	Content* operator-(const Content&) const;
	Content* operator*(const Content&) const;
	Content* operator/(const Content&) const;


// -- Methods

	virtual void   print(ostream&) const  = 0;
	virtual string typeName()      const = 0;

	virtual bool   isNil()     const  { return false; }
	virtual bool   isNumber()  const  { return false; }
	virtual bool   isString()  const  { return false; }
	virtual bool   isList()    const  { return false; }
	virtual bool   isDate()    const  { return false; }
	virtual bool   isTime()    const  { return false; }
	virtual bool   isDateTime()const  { return false; }

// -- Overridden methods

	// From Streamble

	virtual void  encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

private:

// -- No copy allowed

	Content(const Content&);
	Content& operator=(const Content&);

// -- Class members

    static  ClassSpec            classSpec_;
    static  Reanimator<Content>  reanimator_;

// -- Methods

	virtual int      compare(const Content&)    const = 0;
	virtual Content* add(const Content&)        const = 0;
	virtual Content* sub(const Content&)        const = 0;
	virtual Content* mul(const Content&)        const = 0;
	virtual Content* div(const Content&)        const = 0;

	void    badConvertion(const string&) const;
	void    badComparison(const string&) const;
	void    badOperator(const string&, const string&) const;

// -- Friends

	friend ostream& operator<<(ostream& s, const Content& content) 
		{ content.print(s); return s; }

	friend class Value;
};

template<>
Streamable* Reanimator<Content>::ressucitate(Stream& s) const
#ifdef IBM
{ return 0;}
#else
;
#endif

#endif
