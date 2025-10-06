/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
