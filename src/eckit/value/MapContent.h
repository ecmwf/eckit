/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Manuel Fuentes
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_MapContent_h
#define eckit_MapContent_h

#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MapContent : public Content {

protected:

    // -- Constructor

    MapContent();
    MapContent(const ValueMap&);

    MapContent(Stream&);

    // -- Destructor

    ~MapContent() override;

    // -- Overridden methods

    // -- From Content

    int compare(const Content& other) const override;

    void value(bool& n) const override { Content::value(n); }
    void value(long long& n) const override { Content::value(n); }
    void value(double& n) const override { Content::value(n); }
    void value(std::string& n) const override { Content::value(n); }
    void value(Date& n) const override { Content::value(n); }
    void value(Time& n) const override { Content::value(n); }
    void value(DateTime& n) const override { Content::value(n); }
    void value(ValueList& n) const override { Content::value(n); }
    void value(ValueMap& n) const override;

    int compareBool(const BoolContent&) const override { return -1; }
    int compareNumber(const NumberContent&) const override { return -1; }
    int compareDouble(const DoubleContent&) const override { return -1; }
    int compareString(const StringContent&) const override { return -1; }
    int compareNil(const NilContent&) const override { return -1; }
    int compareList(const ListContent&) const override { return -1; }
    int compareMap(const MapContent&) const override;
    int compareDate(const DateContent&) const override { return 1; }
    int compareTime(const TimeContent&) const override { return 1; }
    int compareDateTime(const DateTimeContent&) const override { return 1; }
    int compareOrderedMap(const OrderedMapContent&) const override { return 1; }

    Content* add(const Content&) const override;
    Content* sub(const Content&) const override;
    Content* mul(const Content&) const override;
    Content* div(const Content&) const override;
    Content* mod(const Content&) const override;

    Value keys() const override;
    Value& element(const Value&) override;
    bool contains(const Value& key) const override;
    Value remove(const Value&) override;

    void print(std::ostream&) const override;
    void json(JSON&) const override;

    std::string typeName() const override { return "Map"; }

    bool isMap() const override { return true; }
    Content* clone() const override;
    void dump(std::ostream& out, size_t depth, bool indent = true) const override;

    void hash(Hash&) const override;

    // -- From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    ValueMap value_;

    // -- Class Members

    static ClassSpec classSpec_;
    static Reanimator<MapContent> reanimator_;

    // -- Friends

    friend class Reanimator<MapContent>;
    friend class Value;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
