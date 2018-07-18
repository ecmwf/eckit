/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/Expressions.h"
#include "eckit/sql/SQLExpressionEvaluated.h"
#include "eckit/sql/SQLOrderOutput.h"

namespace eckit {
namespace sql {

SQLOrderOutput::SQLOrderOutput(SQLOutput* output, const std::pair<Expressions,std::vector<bool> >& by)
: output_(output),
  by_(by)
{
	eckit::Log::debug() << *this << std::endl;
}

SQLOrderOutput::~SQLOrderOutput()
{
	for (SortedResults::iterator it(sortedResults_.begin()); it != sortedResults_.end(); ++it)
	{
		VectorOfExpressions& v(it->second);
		for (size_t i = 0; i < v.size(); ++i)
		{
			Expressions& es(v[i]);
			for (size_t j = 0; j < es.size(); ++j)
				delete es[j];
		}
	}
}

void SQLOrderOutput::print(std::ostream& s) const
{
	s << "SQLOrderOutput[" << *output_ << " ORDER BY ";
	for(size_t i = 0; i < by_.first.size(); i++)
		s << *(by_.first[i]) << (by_.second[i] ? " ASC " : " DESC ") << ", ";
	s << "]";
}

void SQLOrderOutput::size(int count) { output_->size(count); }

unsigned long long SQLOrderOutput::count() { return output_->count(); }

void SQLOrderOutput::reset() { output_->reset(); }

void SQLOrderOutput::flush()
{
	for (SortedResults::iterator it = sortedResults_.begin(); it != sortedResults_.end(); ++it)
	{
		std::vector<Expressions>& rows = it->second;
		for (size_t i = 0; i < rows.size(); ++i)
            output_->output(rows[i]);
	}
    output_->flush();
}

bool SQLOrderOutput::output(const Expressions& results)
{
	OrderByExpressions byValues(by_.second);
    Expressions& byExpressions(by_.first);
	for (size_t i = 0; i < byExpressions.size(); ++i)
		byValues.push_back(new SQLExpressionEvaluated(
            byIndices_[i]
            ? *results[byIndices_[i] - 1]
            : *byExpressions[i]));

	Expressions resultValues;
	for (size_t i = 0; i < results.size(); ++i)
		resultValues.push_back(new SQLExpressionEvaluated(*results[i]));

	sortedResults_[byValues].push_back(resultValues);
	return false;
}

void SQLOrderOutput::prepare(SQLSelect& sql)
{
	output_->prepare(sql);
    Expressions& ex(by_.first);
    for(size_t i(0); i < ex.size(); ++i)
    {
        if (! ex[i]->isConstant())
        {
            ex[i]->prepare(sql);
            byIndices_.push_back(0);
        }
        else
        {
            bool missing(false);
            size_t index(ex[i]->eval(missing));
            ASSERT(! missing);
            if (index < 1) throw eckit::UserError("ORDER BY: indices of columns must be positive");
            byIndices_.push_back(index);
        } 
    }
}

void SQLOrderOutput::cleanup(SQLSelect& sql)
{
	output_->cleanup(sql);
	for(Expressions::iterator j = by_.first.begin(); j != by_.first.end() ; ++j)
		(*j)->cleanup(sql);
}

const SQLOutputConfig& SQLOrderOutput::config() { return output_->config(); }
void SQLOrderOutput::config(SQLOutputConfig& cfg) { output_->config(cfg); }


} // namespace sql
} // namespace eckit
