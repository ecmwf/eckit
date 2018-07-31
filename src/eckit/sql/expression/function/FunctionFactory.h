/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionFactory.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef odb_api_FunctionFactory_H
#define odb_api_FunctionFactory_H

#include "odb_api/FunctionExpression.h"
#include "eckit/sql/SQLAST.h"

namespace eckit { namespace sql { class SelectAST; } }

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionFactoryBase {
protected:
	int    arity_;
	std::string name_;
	std::string help_;
	virtual FunctionExpression* make(const std::string&,const expression::Expressions&) = 0;

public:
	//FunctionFactoryBase() : name_("FunctionFactory"), arity_(-1) {}
	FunctionFactoryBase(const std::string& name, int arity, const std::string& help);
	~FunctionFactoryBase();

	FunctionExpression* build(const std::string&, SQLExpression*);
	FunctionExpression* build(const std::string&, SQLExpression*, SQLExpression*);
	FunctionExpression* build(const std::string&, SQLExpression*, SQLExpression*, SQLExpression*);
	FunctionExpression* build(const std::string&, const expression::Expressions&);
    FunctionExpression* build(const std::string&, const expression::Expressions&, const SelectAST&);
};

class FunctionFactory : public FunctionFactoryBase {
public:
	static FunctionFactory& instance();
	FunctionFactory(); // : FunctionFactoryBase("FunctionFactory", -1) {}

    typedef std::vector<std::pair<std::pair<std::string, int>, std::string> > FunctionInfo;

	FunctionInfo& functionsInfo();

private:
	FunctionExpression* make(const std::string&,const expression::Expressions&) { NOTIMP; return 0; }


    std::map<std::pair<std::string,int>, FunctionFactoryBase*> map_;
	FunctionInfo functionInfo_;
};

template<class T>
class FunctionMaker : public FunctionFactoryBase {
	FunctionExpression* make(const std::string& name, const expression::Expressions& args)
	{ return new T(name, args); }
public:
	FunctionMaker(const std::string& name, int arity, const std::string& help) : FunctionFactoryBase(name, arity, help) {}
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
