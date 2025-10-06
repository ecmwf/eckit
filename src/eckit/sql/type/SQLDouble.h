/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
