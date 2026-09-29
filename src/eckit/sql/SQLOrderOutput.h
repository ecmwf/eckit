// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Nov 2011
/// @date Aug 2018

#ifndef eckit_sql_SQLOrderOutput_H
#define eckit_sql_SQLOrderOutput_H

#include <queue>

#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/expression/OrderByExpressions.h"
#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLOrderOutput : public SQLOutput {
public:

    SQLOrderOutput(SQLOutput& output, const std::pair<expression::Expressions, std::vector<bool>>& by);
    ~SQLOrderOutput() override;

private:  // methods

    void print(std::ostream&) const override;

    // -- Members

    SQLOutput& output_;
    std::pair<expression::Expressions, std::vector<bool>> by_;

    using SortedResults = std::map<expression::OrderByExpressions, std::queue<Expressions>>;

    SortedResults sortedResults_;
    std::vector<size_t> byIndices_;

    // -- Overridden methods
    void reset() override;
    void flush() override;

    /// OrderBy builds a std::map of sorted results. Now we start outputting them.
    bool cachedNext() override;

    bool output(const expression::Expressions&) override;
    void preprepare(SQLSelect&) override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    unsigned long long count() override;

    // Overridden (and removed) functions

    void outputReal(double, bool) override;
    void outputDouble(double, bool) override;
    void outputInt(double, bool) override;
    void outputUnsignedInt(double, bool) override;
    void outputString(const char*, size_t, bool) override;
    void outputBitfield(double, bool) override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
