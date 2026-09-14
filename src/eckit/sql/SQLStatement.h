// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLStatement.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLStatement_H
#define SQLStatement_H

#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit::sql {
// Forward declarations

class SQLDatabase;

class SQLStatement {
public:

    SQLStatement();
    virtual ~SQLStatement();

    virtual unsigned long long execute()           = 0;
    virtual expression::Expressions output() const = 0;

protected:

    virtual void print(std::ostream&) const;

private:

    // No copy allowed
    SQLStatement(const SQLStatement&);
    SQLStatement& operator=(const SQLStatement&);


    friend std::ostream& operator<<(std::ostream& s, const SQLStatement& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit::sql

#endif
