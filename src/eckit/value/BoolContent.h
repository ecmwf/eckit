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

#ifndef eckit_BoolContent_h
#define eckit_BoolContent_h

#include "eckit/value/Content.h"
#include "eckit/value/Value.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class BoolContent : public Content {

protected:

    // -- Constructor

    BoolContent(bool);
    BoolContent(Stream&);

    // -- Destructor

    ~BoolContent() override;

    // -- Overridden methods

    // -- From Content

    int compare(const Content& other) const override;

    void value(bool& n) const override;
    void value(long long& n) const override;
    void value(double& n) const override;
    void value(std::string& n) const override;
    void value(Date& n) const override { Content::value(n); }
    void value(Time& n) const override { Content::value(n); }
    void value(DateTime& n) const override { Content::value(n); }
    void value(ValueList& n) const override { Content::value(n); }
    void value(ValueMap& n) const override { Content::value(n); }

    int compareBool(const BoolContent&) const override;
    int compareNumber(const NumberContent&) const override { return 1; }
    int compareDouble(const DoubleContent&) const override { return 1; }
    int compareString(const StringContent&) const override { return 1; }
    int compareNil(const NilContent&) const override { return 1; }
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

    //        Content* addBool(const BoolContent&) const override;
    //        Content* subBool(const BoolContent&) const override;
    //        Content* mulBool(const BoolContent&) const override;
    //        Content* divBool(const BoolContent&) const override;

    //        Content* addNumber(const NumberContent&) const override;
    //        Content* subNumber(const NumberContent&) const override;
    //        Content* mulNumber(const NumberContent&) const override;
    //        Content* divNumber(const NumberContent&) const override;

    void print(std::ostream&) const override;
    void dump(std::ostream& out, size_t depth, bool indent = true) const override;

    void json(JSON&) const override;
    std::string typeName() const override { return "Bool"; }
    bool isBool() const override { return true; }
    Content* clone() const override;

    void hash(Hash&) const override;

    // -- From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    BoolContent(const BoolContent&);
    BoolContent& operator=(const BoolContent&);

    // -- Members

    bool value_;

    // -- Class Members

    static ClassSpec classSpec_;
    static Reanimator<BoolContent> reanimator_;

    // -- Friends

    friend class Reanimator<BoolContent>;
    friend class Value;
    friend class DateContent;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
