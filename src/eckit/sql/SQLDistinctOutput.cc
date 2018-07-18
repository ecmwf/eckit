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
#include "odb_api/Expressions.h"

namespace eckit {
namespace sql {

SQLDistinctOutput::SQLDistinctOutput(SQLOutput* output)
: output_(output)
{} 

SQLDistinctOutput::~SQLDistinctOutput() {}

const SQLOutputConfig& SQLDistinctOutput::config() { return output_->config(); }
void SQLDistinctOutput::config(SQLOutputConfig& cfg) { output_->config(cfg); }

void SQLDistinctOutput::print(std::ostream& s) const
{
	s << "SQLDistinctOutput[" << *output_ << "]";
}

void SQLDistinctOutput::size(int count)
{
	output_->size(count);
	tmp_ = std::vector<double>(count);
}

void SQLDistinctOutput::reset()
{
	output_->reset();
	seen_.clear();
}

void SQLDistinctOutput::flush() { output_->flush(); }

unsigned long long SQLDistinctOutput::count() { return output_->count(); }

bool SQLDistinctOutput::output(const expression::Expressions& results)
{
	for(size_t i = 0; i < results.size(); i++)
	{
		bool missing = false;
		tmp_[i] = results[i]->eval(missing);
		// What do we do with missing? Or has it been already evaluated somewhere before and it doesn't matter???...
	}

	if(seen_.find(tmp_) == seen_.end())
	{
		seen_.insert(tmp_);
        output_->output(results);
		return true;
	}
	return false;
}

void SQLDistinctOutput::prepare(SQLSelect& sql) { output_->prepare(sql); }

void SQLDistinctOutput::cleanup(SQLSelect& sql) { output_->cleanup(sql); }

} // namespace sql
} // namespace eckit
