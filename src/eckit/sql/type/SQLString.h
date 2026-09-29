// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLString.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLString_H
#define SQLString_H

#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {

class SQLOutput;

namespace type {

class SQLString : public SQLType {
public:

    SQLString(const std::string& name, size_t maxLen);
    ~SQLString();

private:

    // No copy allowed
    SQLString(const SQLString&);
    SQLString& operator=(const SQLString&);

    // -- Overridden methods
    size_t size() const override;
    void output(SQLOutput&, double, bool) const override;
    void output(SQLOutput&, const double*, bool) const override;
    std::string asString(const double* val) const override;
    int getKind() const override { return stringType; }
    manipulator format() const override;

    size_t width() const override;

    // friend std::ostream& operator<<(std::ostream& s,const SQLString& p)
    //	{ p.print(s); return s; }

    size_t maxLen_;
};

}  // namespace type
}  // namespace eckit::sql

#endif
