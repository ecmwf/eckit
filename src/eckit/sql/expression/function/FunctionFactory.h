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

#ifndef eckit_sql_expression_FunctionFactory_H
#define eckit_sql_expression_FunctionFactory_H

#include <memory>

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionFactoryBase {
protected:
	int    arity_;
	std::string name_;
	std::string help_;
    virtual std::shared_ptr<FunctionExpression> make(const std::string&,const expression::Expressions&) = 0;

public:
	//FunctionFactoryBase() : name_("FunctionFactory"), arity_(-1) {}
	FunctionFactoryBase(const std::string& name, int arity, const std::string& help);
    virtual ~FunctionFactoryBase();

    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, const expression::Expressions&);
};

class FunctionFactory : public FunctionFactoryBase {
public:
	static FunctionFactory& instance();
	FunctionFactory(); // : FunctionFactoryBase("FunctionFactory", -1) {}
    virtual ~FunctionFactory() {}

    typedef std::vector<std::pair<std::pair<std::string, int>, std::string> > FunctionInfo;

	FunctionInfo& functionsInfo();

private:
    std::shared_ptr<FunctionExpression> make(const std::string&,const expression::Expressions&) { NOTIMP; return 0; }


    std::map<std::pair<std::string,int>, FunctionFactoryBase*> map_;
	FunctionInfo functionInfo_;
};

template<class T>
class FunctionMaker : public FunctionFactoryBase {
    std::shared_ptr<FunctionExpression> make(const std::string& name, const expression::Expressions& args) {
        return std::make_shared<T>(name, args);
    }
public:
    FunctionMaker(const std::string& name, int arity, const std::string& help) :
        FunctionFactoryBase(name, arity, help) {}
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace eckit

#endif
