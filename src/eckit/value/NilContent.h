/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Baudouin Raoult
/// @author Manuel Fuentes

#ifndef eckit_NilContent_h
#define eckit_NilContent_h

#include "eckit/value/Content.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class NilContent : public Content {
protected:
    // -- Constructors

    NilContent();
    NilContent(Stream&);

    // -- Destructors

    virtual ~NilContent() override;

    // -- Overridden Methods

    // From Content

    virtual int compare(const Content& other) const override;

    virtual void value(bool& n) const override { Content::value(n); }
    virtual void value(long long& n) const override { Content::value(n); }
    virtual void value(double& n) const override { Content::value(n); }
    virtual void value(std::string& n) const override { Content::value(n); }
    virtual void value(Date& n) const override { Content::value(n); }
    virtual void value(Time& n) const override { Content::value(n); }
    virtual void value(DateTime& n) const override { Content::value(n); }
    virtual void value(ValueList& n) const override;
    virtual void value(ValueMap& n) const override { Content::value(n); }

    virtual int compareBool(const BoolContent&) const override { return -1; }
    virtual int compareNumber(const NumberContent&) const override { return -1; }
    virtual int compareDouble(const DoubleContent&) const override { return -1; }
    virtual int compareString(const StringContent&) const override { return -1; }
    virtual int compareNil(const NilContent&) const override;
    virtual int compareList(const ListContent&) const override { return 1; }
    virtual int compareMap(const MapContent&) const override { return 1; }
    virtual int compareDate(const DateContent&) const override { return 1; }
    virtual int compareTime(const TimeContent&) const override { return 1; }
    virtual int compareDateTime(const DateTimeContent&) const override { return 1; }
    virtual int compareOrderedMap(const OrderedMapContent&) const override { return 1; }

    virtual Content* add(const Content&) const override;
    virtual Content* sub(const Content&) const override;
    virtual Content* mul(const Content&) const override;
    virtual Content* div(const Content&) const override;
    virtual Content* mod(const Content&) const override;

    virtual Content* addNil(const NilContent&) const override;
    virtual Content* subNil(const NilContent&) const override;
    virtual Content* mulNil(const NilContent&) const override;
    virtual Content* divNil(const NilContent&) const override;

    virtual bool isNil() const override { return true; }
    virtual std::string typeName() const override { return "Nil"; }
    virtual void print(std::ostream&) const override;
    virtual void json(JSON&) const override;
    virtual Content* clone() const override;
    virtual void dump(std::ostream& out, size_t depth, bool indent = true) const override;

    virtual bool contains(const Value&) const override;

    // From Streamable

    virtual void encode(Stream&) const override;
    virtual const ReanimatorBase& reanimator() const override { return reanimator_; }

    virtual void hash(Hash&) const override;

private:
    // -- No copy allowed

    NilContent(const NilContent&);
    NilContent& operator=(const NilContent&);

    // -- Class Members

    static ClassSpec classSpec_;
    static Reanimator<NilContent> reanimator_;

    // -- Friends

    friend class Reanimator<NilContent>;
    friend class Value;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
