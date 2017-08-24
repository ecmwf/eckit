/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File OrderedMapContent.h
// Manuel Fuentes - ECMWF Jun 97

#ifndef eckit_OrderedMapContent_h
#define eckit_OrderedMapContent_h

#include "eckit/value/MapContent.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class OrderedMapContent : public MapContent {

protected:

// -- Constructor

    OrderedMapContent();
    OrderedMapContent(const ValueMap&, const ValueList&);
    //OrderedMapContent(const Value&);

    OrderedMapContent(Stream&);

// -- Destructor

    virtual ~OrderedMapContent();

// -- Overridden methods

    // -- From Content


    virtual void value(ValueMap& v) const;

    virtual Value   keys() const;

//    virtual Content* addMap(const OrderedMapContent&) const;

    virtual void   print(std::ostream&) const;
    virtual void   json(JSON&)     const;
    virtual std::string typeName() const       { return "OrderedMap"; }

    virtual Content* clone() const;
    virtual void    dump(std::ostream& out, size_t depth, bool indent = true) const;
    virtual Value&   element(const Value&);

    // -- From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()         { return classSpec_;}

private:

    OrderedMapContent(const OrderedMapContent&);
    OrderedMapContent& operator=(const OrderedMapContent&);

// -- Methids

    const Value& value(const Value& key) const;

// -- Members

    ValueMap value_;
    ValueList keys_;

// -- Class Members

    static  ClassSpec                  classSpec_;
    static  Reanimator<OrderedMapContent>  reanimator_;

// -- Friends

    friend class Reanimator<OrderedMapContent>;
    friend class Value;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
