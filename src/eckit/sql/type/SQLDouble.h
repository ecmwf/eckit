// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLDouble.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLDouble_H
#define SQLDouble_H

#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {

class SQLOutput;

namespace type {

class SQLDouble : public SQLType {
public:

    SQLDouble(const std::string&);
    ~SQLDouble();

    // -- Overridden methods
    using SQLType::output;
    void output(SQLOutput&, double, bool) const override;

private:

    // No copy allowed
    SQLDouble(const SQLDouble&);
    SQLDouble& operator=(const SQLDouble&);

    size_t size() const override;
    int getKind() const override { return doubleType; }
    std::string asString(const double* val) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const SQLDouble& p)
    //	{ p.print(s); return s; }
};

}  // namespace type
}  // namespace eckit::sql

#endif
