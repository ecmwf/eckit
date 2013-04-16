/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StringContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eckit_StringContent_h
#define eckit_StringContent_h

#include "eckit/value/Content.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class StringContent : public Content {

protected:

    // -- Constructor

    StringContent(const string&);
    StringContent(const char*);
    StringContent(Stream&);

    // -- Destructor

    virtual ~StringContent();

    // -- Overridden methods

    // -- From Content

    virtual int compare(const Content& other) const;

    virtual void value(bool& n)                 const;
    virtual void value(long long& n)            const { Content::value(n); }
    virtual void value(double& n)               const { Content::value(n); }
    virtual void value(string& n)               const;
    virtual void value(Date& n)                 const { Content::value(n); }
    virtual void value(Time& n)                 const { Content::value(n); }
    virtual void value(DateTime& n)             const { Content::value(n); }
    virtual void value(vector<Value>& n)        const { Content::value(n); }
    virtual void value(map<Value,Value>& n)     const { Content::value(n); }

    virtual int  compareBool(const BoolContent&)            const {return -1; }
    virtual int  compareNumber(const NumberContent&)        const {return -1; }
    virtual int  compareDouble(const DoubleContent&)        const {return -1; }
    virtual int  compareString(const StringContent&)        const;
    virtual int  compareNil(const NilContent&)              const {return 1; }
    virtual int  compareList(const ListContent&)            const {return 1; }
    virtual int  compareMap(const MapContent&)              const {return 1; }
    virtual int  compareDate(const DateContent&)            const {return 1; }
    virtual int  compareTime(const TimeContent&)            const {return 1; }
    virtual int  compareDateTime(const DateTimeContent&)    const {return 1; }

    virtual Content* add(const Content&)             const;
    virtual Content* sub(const Content&) const;
    virtual Content* mul(const Content&) const;
    virtual Content* div(const Content&) const;
    virtual Content* mod(const Content&) const;

    virtual Content* addString(const StringContent&) const;

    virtual void    print(ostream&) const;
    virtual void   json(JSON&)     const;
    virtual string  typeName() const      { return "String"; }
    virtual bool    isString() const      { return true; }

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

    // -- Class methods

    static  const ClassSpec&  classSpec()         { return classSpec_;}

private:

    // -- No copy allowed

    StringContent(const StringContent&);
    StringContent& operator=(const StringContent&);

    // -- Members

    string value_;

    static  ClassSpec                  classSpec_;
    static  Reanimator<StringContent>  reanimator_;

    // -- Friends

    friend class Reanimator<StringContent>;
    friend class Value;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
