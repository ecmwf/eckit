// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLReal.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLReal_H
#define SQLReal_H

#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {

class SQLOutput;

namespace type {

class SQLReal : public SQLType {
public:

    // -- Exceptions
    // None

    // -- Contructors

    SQLReal(const std::string&);

    // -- Destructor

    ~SQLReal();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    using SQLType::output;
    void output(SQLOutput&, double, bool) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    SQLReal(const SQLReal&);
    SQLReal& operator=(const SQLReal&);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    size_t size() const override;
    int getKind() const override { return realType; }
    std::string asString(const double* val) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const SQLReal& p)
    //	{ p.print(s); return s; }
};

}  // namespace type
}  // namespace eckit::sql

#endif
