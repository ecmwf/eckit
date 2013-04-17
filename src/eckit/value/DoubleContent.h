/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File DoubleContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eckit_DoubleContent_h
#define eckit_DoubleContent_h

#include "eckit/value/Content.h"
#include "eckit/value/Value.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class DoubleContent : public Content {

protected:

// -- Constructor

        DoubleContent(double);
        DoubleContent(Stream&);

// -- Destructor

        virtual ~DoubleContent();

// -- Overridden methods

	// -- From Content
        virtual int compare(const Content& other) const;

        virtual void value(bool& n)                 const { Content::value(n); }
        virtual void value(long long& n)            const { Content::value(n); }
        virtual void value(double& n)               const;
        virtual void value(string& n)               const;
        virtual void value(Date& n)                 const { Content::value(n); }
        virtual void value(Time& n)                 const { Content::value(n); }
        virtual void value(DateTime& n)             const { Content::value(n); }
        virtual void value(vector<Value>& n)        const { Content::value(n); }
        virtual void value(map<Value,Value>& n)     const { Content::value(n); }

        virtual int  compareBool(const BoolContent&)            const {return -1; }
        virtual int  compareNumber(const NumberContent&)        const;
        virtual int  compareDouble(const DoubleContent&)        const;
        virtual int  compareString(const StringContent&)        const {return 1; }
        virtual int  compareNil(const NilContent&)              const {return 1; }
        virtual int  compareList(const ListContent&)            const {return 1; }
        virtual int  compareMap(const MapContent&)              const {return 1; }
        virtual int  compareDate(const DateContent&)            const {return 1; }
        virtual int  compareTime(const TimeContent&)            const {return 1; }
        virtual int  compareDateTime(const DateTimeContent&)    const {return 1; }


        virtual Content* add(const Content&) const;
        virtual Content* sub(const Content&) const;
        virtual Content* mul(const Content&) const;
        virtual Content* div(const Content&) const;
        virtual Content* mod(const Content&) const;

        virtual Content* addDouble(const DoubleContent&) const;
        virtual Content* subDouble(const DoubleContent&) const;
        virtual Content* mulDouble(const DoubleContent&) const;
        virtual Content* divDouble(const DoubleContent&) const;


        virtual Value negate() const;

//        virtual Content* addNumber(const NumberContent&) const;
//        virtual Content* subNumber(const NumberContent&) const;
//        virtual Content* mulNumber(const NumberContent&) const;
//        virtual Content* divNumber(const NumberContent&) const;

        virtual void    print(ostream&) const;
        virtual void   json(JSON&)      const;
        virtual string  typeName()      const { return "Double"; }
        virtual bool    isDouble()      const { return true; }

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()            { return classSpec_;}

private:

        DoubleContent(const DoubleContent&);
        DoubleContent& operator=(const DoubleContent&);

// -- Members

        double value_;

// -- Class Members

	static  ClassSpec                  classSpec_;
    static  Reanimator<DoubleContent>  reanimator_;

// -- Friends

        friend class Reanimator<DoubleContent>;
	friend class Value;
	friend class DateContent;
    friend class NumberContent;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
