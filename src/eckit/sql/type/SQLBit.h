// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
