/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLBitColumn.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBitColumn_H
#define SQLBitColumn_H

#include "eckit/sql/SQLColumn.h"

namespace eckit::sql {

class SQLBitColumn : public SQLColumn {
public:

    SQLBitColumn(const SQLColumn&, const std::string&);
    ~SQLBitColumn();

private:

    // No copy allowed
    SQLBitColumn(const SQLBitColumn&);
    SQLBitColumn& operator=(const SQLBitColumn&);

    std::string field_;
    unsigned long mask_;
    unsigned long shift_;

    // -- Overridden methods

    // From ODBIterator
    void rewind() override;
    double next(bool& missing) override;
    void advance(unsigned long) override;
    void print(std::ostream&) const override;

    // friend std::ostream& operator<<(std::ostream& s,const SQLBitColumn& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql

#endif
