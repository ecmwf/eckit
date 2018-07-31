/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/FunctionEQ.h"
#include "odb_api/FunctionNOT_IN.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

FunctionNOT_IN::FunctionNOT_IN(const std::string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  size_(args.size() - 1)
{}

FunctionNOT_IN::FunctionNOT_IN(const FunctionNOT_IN& other)
: FunctionExpression(other.name_, other.args_),
  size_(other.args_.size() - 1)
{}

FunctionNOT_IN::~FunctionNOT_IN() {}

SQLExpression* FunctionNOT_IN::clone() const { return new FunctionNOT_IN(*this); }

const type::SQLType* FunctionNOT_IN::type() const { return &type::SQLType::lookup("double"); }

double FunctionNOT_IN::eval(bool& missing) const
{
	const SQLExpression& x = *args_[size_];
	for(size_t i = 0; i < size_; ++i)
	{
		double y = args_[i]->eval(missing);
		if (FunctionEQ::equal(x, *args_[i], missing))
			return false;
	}

	return true;
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit

