/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File BoolContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eclib_BoolContent_h
#define eclib_BoolContent_h

#include "eclib/Content.h"
#include "eclib/Value.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class BoolContent : public Content {

protected:

    // -- Constructor

    BoolContent(bool);
    BoolContent(Stream&);

    // -- Destructor

    virtual ~BoolContent();

    // -- Overridden methods

    // -- From Content

    virtual int compare(const Content& other) const;

    virtual void value(bool& n)                 const;
    virtual void value(long long& n)            const;
    virtual void value(double& n)               const;
    virtual void value(string& n)               const;
    virtual void value(Date& n)                 const { Content::value(n); }
    virtual void value(Time& n)                 const { Content::value(n); }
    virtual void value(DateTime& n)             const { Content::value(n); }
    virtual void value(vector<Value>& n)        const { Content::value(n); }
    virtual void value(map<Value,Value>& n)     const { Content::value(n); }

    virtual int  compareBool(const BoolContent&)            const;
    virtual int  compareNumber(const NumberContent&)        const {return 1; }
    virtual int  compareDouble(const DoubleContent&)        const {return 1; }
    virtual int  compareString(const StringContent&)        const {return 1; }
    virtual int  compareNil(const NilContent&)              const {return 1; }
    virtual int  compareList(const ListContent&)            const {return 1; }
    virtual int  compareMap(const MapContent&)              const {return 1; }
    virtual int  compareDate(const DateContent&)            const {return 1; }
    virtual int  compareTime(const TimeContent&)            const {return 1; }
    virtual int  compareDateTime(const DateTimeContent&)    const {return 1; }

    virtual Content* add(const Content&)  const;
    virtual Content* sub(const Content&) const;
    virtual Content* mul(const Content&) const;
    virtual Content* div(const Content&) const;
    virtual Content* mod(const Content&) const;

    //        virtual Content* addBool(const BoolContent&) const;
    //        virtual Content* subBool(const BoolContent&) const;
    //        virtual Content* mulBool(const BoolContent&) const;
    //        virtual Content* divBool(const BoolContent&) const;

    //        virtual Content* addNumber(const NumberContent&) const;
    //        virtual Content* subNumber(const NumberContent&) const;
    //        virtual Content* mulNumber(const NumberContent&) const;
    //        virtual Content* divNumber(const NumberContent&) const;

    virtual void    print(ostream&) const;
    virtual void   json(JSON&)     const;
    virtual string  typeName()      const { return "Bool"; }
    virtual bool    isBool()      const { return true; }

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

    // -- Class methods

    static  const ClassSpec&  classSpec()            { return classSpec_;}

private:

    BoolContent(const BoolContent&);
    BoolContent& operator=(const BoolContent&);

    // -- Members

    bool value_;

    // -- Class Members

    static  ClassSpec                  classSpec_;
    static  Reanimator<BoolContent>  reanimator_;

    // -- Friends

    friend class Reanimator<BoolContent>;
    friend class Value;
    friend class DateContent;
};

//-----------------------------------------------------------------------------

} // namespace eclib

#endif
