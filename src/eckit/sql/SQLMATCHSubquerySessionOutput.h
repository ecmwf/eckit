/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
