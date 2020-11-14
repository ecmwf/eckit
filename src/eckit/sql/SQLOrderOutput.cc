/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLOrderOutput.h"
#include "eckit/sql/expression/SQLExpressionEvaluated.h"

using namespace eckit::sql::expression;

/// @note This code triggers a segmentation fault on the CRAY CC compiler 8.4 when optimisation is turned on
///       The compiler bug has been fixed in versions >= 8.5
///       We have chosen to turn off optimisations for all CRAY compilers below, since this is not perfromance critical
/// See ECKIT-405 ECKIT-406
#ifdef _CRAYCC
#pragma _CRI noopt
#endif


namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLOrderOutput::SQLOrderOutput(SQLOutput& output, const std::pair<Expressions, std::vector<bool>>& by) :
    output_(output), by_(by) {}

SQLOrderOutput::~SQLOrderOutput() {}

void SQLOrderOutput::print(std::ostream& s) const {
    s << "SQLOrderOutput[" << output_ << " ORDER BY ";
    for (size_t i = 0; i < by_.first.size(); i++)
        s << *(by_.first[i]) << (by_.second[i] ? " ASC " : " DESC ") << ", ";
    s << "]";
}

unsigned long long SQLOrderOutput::count() {
    return output_.count();
}

void SQLOrderOutput::reset() {
    output_.reset();
}

void SQLOrderOutput::flush() {
    output_.flush();
}

bool SQLOrderOutput::cachedNext() {

    while (true) {

        auto it = sortedResults_.begin();

        // If there are no more results, we are done

        if (it == sortedResults_.end())
            return false;

        // Given identical sorted keys, we use the order that rows are appended

        std::queue<Expressions>& rows = it->second;
        ASSERT(rows.size() >= 1);
        bool success = output_.output(rows.front());

        // Remove entries that have been output

        rows.pop();
        if (rows.empty())
            sortedResults_.erase(it);

        if (success)
            return true;
    }
}

bool SQLOrderOutput::output(const Expressions& results) {
    OrderByExpressions byValues(by_.second);
    Expressions& byExpressions(by_.first);
    for (size_t i = 0; i < byExpressions.size(); ++i) {
        byValues.push_back(
            std::make_shared<SQLExpressionEvaluated>(byIndices_[i] ? *results[byIndices_[i] - 1] : *byExpressions[i]));
    }

    Expressions resultValues;
    for (size_t i = 0; i < results.size(); ++i) {
        resultValues.push_back(std::make_shared<SQLExpressionEvaluated>(*results[i]));
    }

    sortedResults_[byValues].push(resultValues);
    return false;
}

void SQLOrderOutput::preprepare(SQLSelect& sql) {
    output_.preprepare(sql);

    for (auto& exprn : by_.first) {
        exprn->preprepare(sql);
    }
}

void SQLOrderOutput::prepare(SQLSelect& sql) {
    output_.prepare(sql);
    Expressions& ex(by_.first);
    for (size_t i(0); i < ex.size(); ++i) {
        if (!ex[i]->isConstant()) {
            ex[i]->prepare(sql);
            byIndices_.push_back(0);
        }
        else {
            bool missing(false);
            size_t index(ex[i]->eval(missing));
            ASSERT(!missing);
            if (index < 1)
                throw eckit::UserError("ORDER BY: indices of columns must be positive");
            byIndices_.push_back(index);
        }
    }
}

void SQLOrderOutput::cleanup(SQLSelect& sql) {
    output_.cleanup(sql);
    for (Expressions::iterator j = by_.first.begin(); j != by_.first.end(); ++j)
        (*j)->cleanup(sql);
}

// Direct output functions removed in order output

void SQLOrderOutput::outputReal(double, bool) {
    NOTIMP;
}
void SQLOrderOutput::outputDouble(double, bool) {
    NOTIMP;
}
void SQLOrderOutput::outputInt(double, bool) {
    NOTIMP;
}
void SQLOrderOutput::outputUnsignedInt(double, bool) {
    NOTIMP;
}
void SQLOrderOutput::outputString(const char*, size_t, bool) {
    NOTIMP;
}
void SQLOrderOutput::outputBitfield(double, bool) {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
