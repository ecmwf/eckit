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

namespace eckit {
namespace sql {
namespace type {

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

    bool returnAsDouble_;
    unsigned long mask_;
    unsigned long shift_;

    // -- Overridden methods
    // None

    virtual size_t size() const override;
    virtual void output(SQLOutput&, double, bool) const override;
    virtual int getKind() const override { return integerType; }
    virtual std::string asString(const double* val) const override;

    // friend std::ostream& operator<<(std::ostream& s,const SQLBit& p)
    //	{ p.print(s); return s; }
};

}  // namespace type
}  // namespace sql
}  // namespace eckit

#endif
