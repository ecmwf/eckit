// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// \file SQLMATCHSubquerySessionOutput.h
/// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLMATCHSubquerySessionOutput_H
#define SQLMATCHSubquerySessionOutput_H

#include "eckit/sql/SQLOutput.h"

class SelectIterator;
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

class ReaderIterator;

class SQLMATCHSubquerySessionOutput : public SQLOutput {
public:

    SQLMATCHSubquerySessionOutput(odb::sql::expression::function::FunctionMATCH&);
    SQLMATCHSubquerySessionOutput(const SQLMATCHSubquerySessionOutput&);
    ~SQLMATCHSubquerySessionOutput() override;

    SQLMATCHSubquerySessionOutput& operator=(const SQLMATCHSubquerySessionOutput&);

protected:

    void print(std::ostream&) const override;


    // -- Members
    odb::sql::expression::function::FunctionMATCH& f_;

    // -- Methods
    // None

    // -- Overridden methods
    virtual unsigned long long count();
    virtual void size(int);
    virtual void reset();
    void flush() override;
    virtual bool output(const expression::Expressions&);
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;

    virtual void outputReal(double, bool) { NOTIMP; };
    virtual void outputDouble(double, bool) { NOTIMP; };
    virtual void outputInt(double, bool) { NOTIMP; };
    virtual void outputUnsignedInt(double, bool) { NOTIMP; };
    virtual void outputString(double, bool) { NOTIMP; };
    virtual void outputBitfield(double, bool) { NOTIMP; };

private:

    unsigned long long count_;
};

}  // namespace sql
}  // namespace eckit

#endif
