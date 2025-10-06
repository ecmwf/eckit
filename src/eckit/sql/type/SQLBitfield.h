/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLBitfield.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBitfield_H
#define SQLBitfield_H

#include "eckit/sql/SQLTypedefs.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit::sql::type {

class SQLBitfield : public SQLType {
public:

    SQLBitfield(const std::string&, const FieldNames&, const Sizes&);
    ~SQLBitfield();

    unsigned long mask(const std::string& n) const;
    unsigned long shift(const std::string& n) const;

    const BitfieldDef& bitfieldDef() const { return bitfieldDef_; }
    const FieldNames& fields() const { return bitfieldDef_.first; }
    const Sizes& sizes() const { return bitfieldDef_.second; }

    static std::string make(const std::string&, const FieldNames&, const Sizes&, const char* aliasName = NULL);

private:

    SQLBitfield(const SQLBitfield&);
    SQLBitfield& operator=(const SQLBitfield&);

    static unsigned long makeMask(unsigned long size);

    BitfieldDef bitfieldDef_;
    std::map<std::string, unsigned long> mask_;
    std::map<std::string, unsigned long> shift_;

    size_t size() const override;
    using SQLType::output;
    void output(SQLOutput& s, double, bool) const override;
    std::string asString(const double* val) const override;
    const SQLType* subType(const std::string&) const override;
    int getKind() const override { return bitmapType; }

    size_t width_;
    size_t width() const override;
    // friend std::ostream& operator<<(std::ostream& s,const SQLBitfield& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::type

#endif
