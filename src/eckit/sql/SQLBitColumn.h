// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
