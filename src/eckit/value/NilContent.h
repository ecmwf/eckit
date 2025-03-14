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

    ~NilContent() override;

    // -- Overridden Methods

    // From Content

    int compare(const Content& other) const override;

    void value(bool& n) const override { Content::value(n); }
    void value(long long& n) const override { Content::value(n); }
    void value(double& n) const override { Content::value(n); }
    void value(std::string& n) const override { Content::value(n); }
    void value(Date& n) const override { Content::value(n); }
    void value(Time& n) const override { Content::value(n); }
    void value(DateTime& n) const override { Content::value(n); }
    void value(ValueList& n) const override;
    void value(ValueMap& n) const override { Content::value(n); }

    int compareBool(const BoolContent&) const override { return -1; }
    int compareNumber(const NumberContent&) const override { return -1; }
    int compareDouble(const DoubleContent&) const override { return -1; }
    int compareString(const StringContent&) const override { return -1; }
    int compareNil(const NilContent&) const override;
    int compareList(const ListContent&) const override { return 1; }
    int compareMap(const MapContent&) const override { return 1; }
    int compareDate(const DateContent&) const override { return 1; }
    int compareTime(const TimeContent&) const override { return 1; }
    int compareDateTime(const DateTimeContent&) const override { return 1; }
    int compareOrderedMap(const OrderedMapContent&) const override { return 1; }

    Content* add(const Content&) const override;
    Content* sub(const Content&) const override;
    Content* mul(const Content&) const override;
    Content* div(const Content&) const override;
    Content* mod(const Content&) const override;

    Content* addNil(const NilContent&) const override;
    Content* subNil(const NilContent&) const override;
    Content* mulNil(const NilContent&) const override;
    Content* divNil(const NilContent&) const override;

    bool isNil() const override { return true; }
    std::string typeName() const override { return "Nil"; }
    void print(std::ostream&) const override;
    void json(JSON&) const override;
    Content* clone() const override;
    void dump(std::ostream& out, size_t depth, bool indent = true) const override;

    bool contains(const Value&) const override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    void hash(Hash&) const override;

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
