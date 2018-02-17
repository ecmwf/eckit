/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ListContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eckit_ListContent_h
#define eckit_ListContent_h

#include "eckit/value/Value.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ListContent : public Content {

protected:

// -- Constructor

	ListContent();
	ListContent(const ValueList&);
	ListContent(const Value&);

	ListContent(Stream&);

// -- Destructor

	virtual ~ListContent();

// -- Overridden methods

    // -- From Content

    virtual int compare(const Content& other) const;

    virtual void value(bool& n)        const;
    virtual void value(long long& n)   const;
    virtual void value(double& n)      const;
    virtual void value(std::string& n) const;
    virtual void value(Date& n)        const;
    virtual void value(Time& n)        const;
    virtual void value(DateTime& n)    const;
    virtual void value(ValueList& n)   const;
    virtual void value(ValueMap& n)    const { Content::value(n); }

    virtual int  compareBool(const BoolContent&)            const {return -1; }
    virtual int  compareNumber(const NumberContent&)        const {return -1; }
    virtual int  compareDouble(const DoubleContent&)        const {return -1; }
    virtual int  compareString(const StringContent&)        const {return -1; }
    virtual int  compareNil(const NilContent&)              const {return -1; }
    virtual int  compareList(const ListContent&)            const;
    virtual int  compareMap(const MapContent&)              const {return 1; }
    virtual int  compareDate(const DateContent&)            const {return 1; }
    virtual int  compareTime(const TimeContent&)            const {return 1; }
    virtual int  compareDateTime(const DateTimeContent&)    const {return 1; }
    virtual int  compareOrderedMap(const OrderedMapContent&) const { return 1; }

    virtual Content* add(const Content&) const;
	virtual Content* sub(const Content&) const;
	virtual Content* mul(const Content&) const;
	virtual Content* div(const Content&) const;
    virtual Content* mod(const Content&) const;

    virtual Content* addList(const ListContent&) const;

	virtual void   print(std::ostream&) const;
    virtual void   json(JSON&)     const;
	virtual std::string typeName() const       { return "List"; }

	virtual bool   isList() const         { return true; }
    virtual Value& element(const Value&);
    virtual bool contains(const Value& key) const;
    virtual Content* clone() const;
    virtual size_t size() const;
    virtual void    dump(std::ostream& out, size_t depth, bool indent=true) const;

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()         { return classSpec_;}

private:

	ListContent(const ListContent&);
	ListContent& operator=(const ListContent&);

// -- Members

    ValueList value_;

// -- Class Members

    static  ClassSpec                  classSpec_;
    static  Reanimator<ListContent>  reanimator_;

// -- Friends

    friend class Reanimator<ListContent>;
	friend class Value;

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
