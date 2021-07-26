/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLDistinctOutput.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLDistinctOutput::SQLDistinctOutput(SQLOutput& output) :
    output_(output) {}

SQLDistinctOutput::~SQLDistinctOutput() {}

void SQLDistinctOutput::print(std::ostream& s) const {
    s << "SQLDistinctOutput[" << output_ << "]";
}

void SQLDistinctOutput::reset() {
    output_.reset();
    seen_.clear();
}

void SQLDistinctOutput::flush() {
    output_.flush();
}

bool SQLDistinctOutput::cachedNext() {
    return output_.cachedNext();
}

unsigned long long SQLDistinctOutput::count() {
    return output_.count();
}

bool SQLDistinctOutput::output(const expression::Expressions& results) {

    // Get the data into a temporary buffer we can compare

    ASSERT(results.size() == offsets_.size());

    for (size_t i = 0; i < results.size(); i++) {
        bool missing = false;
        results[i]->eval(&tmp_[offsets_[i]], missing);
        // What do we do with missing? Or has it been already evaluated somewhere before and it doesn't matter???...
    }

    if (seen_.find(tmp_) == seen_.end()) {
        seen_.insert(tmp_);
        return output_.output(results);
    }

    return false;
}

void SQLDistinctOutput::preprepare(SQLSelect& sql) {
    output_.preprepare(sql);
}

void SQLDistinctOutput::prepare(SQLSelect& sql) {
    output_.prepare(sql);
    updateTypes(sql);
    seen_.clear();
}

void SQLDistinctOutput::updateTypes(SQLSelect& sql) {

    output_.updateTypes(sql);

    // How much space is needed to store each row of selected data
    offsets_.clear();
    size_t offset = 0;

    for (const auto& column : sql.output()) {
        size_t colSizeBytes = column->type()->size();
        ASSERT(colSizeBytes % 8 == 0);
        offsets_.push_back(offset);
        offset += colSizeBytes / 8;
    }

    // And buffers to do the storage

    tmp_.resize(offset);
}

void SQLDistinctOutput::cleanup(SQLSelect& sql) {
    output_.cleanup(sql);
}

// Direct output functions removed in distinct output

void SQLDistinctOutput::outputReal(double, bool) {
    NOTIMP;
}
void SQLDistinctOutput::outputDouble(double, bool) {
    NOTIMP;
}
void SQLDistinctOutput::outputInt(double, bool) {
    NOTIMP;
}
void SQLDistinctOutput::outputUnsignedInt(double, bool) {
    NOTIMP;
}
void SQLDistinctOutput::outputString(const char*, size_t, bool) {
    NOTIMP;
}
void SQLDistinctOutput::outputBitfield(double, bool) {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
