// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SQLMATCHSubquerySession.h
// Piotr Kuchta - ECMWF Octover 2015

#ifndef SQLMATCHSubquerySession_H
#define SQLMATCHSubquerySession_H

#include "eckit/sql/SQLSession.h"

namespace eckit {
class ReaderIterator;
}
namespace eckit {
class SelectIterator;
}
namespace eckit {
namespace sql {
namespace expression {
namespace function {
class FunctionMATCH;
}
}  // namespace expression
}  // namespace sql
}  // namespace eckit

namespace eckit {
namespace sql {

class SQLMATCHSubquerySession : public SQLSession {
public:

    SQLMATCHSubquerySession(expression::function::FunctionMATCH&);
    ~SQLMATCHSubquerySession();

    SQLStatement* statement();

private:

    // No copy allowed
    SQLMATCHSubquerySession(const SQLMATCHSubquerySession&);
    SQLMATCHSubquerySession& operator=(const SQLMATCHSubquerySession&);

    SQLStatement* statement_;
    expression::function::FunctionMATCH& f_;

    // -- Overridden methods
    void statement(SQLStatement*);
    SQLOutput* defaultOutput();

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const SQLMATCHSubquerySession& p)
    //	{ p.print(s); return s; }
};

}  // namespace sql
}  // namespace eckit

#endif
