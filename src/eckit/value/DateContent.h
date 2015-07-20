/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File DateContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eckit_DateContent_h
#define eckit_DateContent_h

#include "eckit/value/Content.h"
#include "eckit/value/Value.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class DateContent : public Content {

protected:

// -- Constructor

	DateContent(const Date&);
	DateContent(Stream&);

// -- Destructor

	virtual ~DateContent();

// -- Overridden methods

	// -- From Content

    virtual int compare(const Content& other) const;

    virtual void value(bool& n)        const { Content::value(n); }
    virtual void value(long long& n)   const { Content::value(n); }
    virtual void value(double& n)      const { Content::value(n); }
    virtual void value(std::string& n) const { Content::value(n); }
    virtual void value(Date& n)        const;
    virtual void value(Time& n)        const { Content::value(n); }
    virtual void value(DateTime& n)    const { Content::value(n); }
    virtual void value(ValueList& n)   const { Content::value(n); }
    virtual void value(ValueMap& n)    const { Content::value(n); }

    virtual int  compareBool(const BoolContent&)            const {return -1; }
    virtual int  compareNumber(const NumberContent&)        const {return -1; }
    virtual int  compareDouble(const DoubleContent&)        const {return -1; }
    virtual int  compareString(const StringContent&)        const {return -1; }
    virtual int  compareNil(const NilContent&)              const {return -1; }
    virtual int  compareList(const ListContent&)            const {return -1; }
    virtual int  compareMap(const MapContent&)              const {return -1; }
    virtual int  compareDate(const DateContent&)            const;
    virtual int  compareTime(const TimeContent&)            const {return 1; }
    virtual int  compareDateTime(const DateTimeContent&)    const {return 1; }

	virtual Content* add(const Content&) const;
	virtual Content* sub(const Content&) const;
	virtual Content* mul(const Content&) const;
	virtual Content* div(const Content&) const;
    virtual Content* mod(const Content&) const;

	virtual Content* addDate(const DateContent&) const;
	virtual Content* subDate(const DateContent&) const;

	virtual void    print(std::ostream&) const;
    virtual void   json(JSON&)     const;
	virtual std::string  typeName() const       { return "Date"; }
	virtual bool    isDate() const         { return true; }
    virtual Content* clone() const;

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()            { return classSpec_;}

private:

	DateContent(const DateContent&);
	DateContent& operator=(const DateContent&);

// -- Members

    Date value_;

// -- Class Members

	static  ClassSpec                classSpec_;
    static  Reanimator<DateContent>  reanimator_;

// -- Friends

	friend class Reanimator<DateContent>;
	friend class Value;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
