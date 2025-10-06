/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLBit.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBit_H
#define SQLBit_H

#include "eckit/sql/type/SQLType.h"

namespace eckit::sql::type {

class SQLBit : public SQLType {
public:

    SQLBit(const std::string&, unsigned long, unsigned long);
    ~SQLBit();

    unsigned long mask() const { return mask_; }
    unsigned long shift() const { return shift_; }

private:

    // No copy allowed
    SQLBit(const SQLBit&);
    SQLBit& operator=(const SQLBit&);

    unsigned long mask_;
    unsigned long shift_;

    // -- Overridden methods
    // None

    size_t size() const override;
    using SQLType::output;
    void output(SQLOutput&, double, bool) const override;
    int getKind() const override { return integerType; }
    std::string asString(const double* val) const override;

    // friend std::ostream& operator<<(std::ostream& s,const SQLBit& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::type

#endif
