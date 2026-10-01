// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLInt.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLInt_H
#define SQLInt_H

#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {

class SQLOutput;

namespace type {

class SQLInt : public SQLType {
public:

    SQLInt(const std::string&);
    ~SQLInt();

private:

    // No copy allowed
    SQLInt(const SQLInt&);
    SQLInt& operator=(const SQLInt&);

    size_t size() const override;
    using SQLType::output;
    void output(SQLOutput& s, double, bool) const override;
    int getKind() const override { return integerType; }
    std::string asString(const double* val) const override;

    // friend std::ostream& operator<<(std::ostream& s,const SQLInt& p)
    //	{ p.print(s); return s; }
};

}  // namespace type
}  // namespace eckit::sql

#endif
