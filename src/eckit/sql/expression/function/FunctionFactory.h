/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// @author Simon Smart
// @date January 2019

// @note Based on
// File FunctionFactory.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef eckit_sql_expression_FunctionFactory_H
#define eckit_sql_expression_FunctionFactory_H

#include <memory>
#include <mutex>

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

//----------------------------------------------------------------------------------------------------------------------

class FunctionBuilderBase;

class FunctionFactory {

public:  // types

    struct FuncInfo {
        std::string name;
        int arity;
        std::string help;
    };

public:  // methods

    static FunctionFactory& instance();

    void enregister(const std::string& name, int arity_, const FunctionBuilderBase* builder);
    void deregister(const std::string& name, int arity_, const FunctionBuilderBase* builder);

    std::vector<FuncInfo> functionsInfo();

    std::shared_ptr<FunctionExpression> build(const std::string& name, const expression::Expressions& args) const;
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>,
                                              std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>,
                                              std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>);

private:  // methods

    FunctionFactory();
    ~FunctionFactory();

private:  // members

    mutable std::mutex m_;

    std::map<std::pair<std::string, int>, const FunctionBuilderBase*> builders_;
};

//----------------------------------------------------------------------------------------------------------------------

class FunctionBuilderBase {

    std::string name_;
    int arity_;
    std::string help_;

public:  // methods

    FunctionBuilderBase(const std::string& name, int arity, const char* help);
    virtual ~FunctionBuilderBase();

    virtual std::shared_ptr<FunctionExpression> make(const std::string& name,
                                                     const expression::Expressions& args) const = 0;
    int arity() const;
    std::string help() const;
};


template <typename FunctionType>
class FunctionBuilder : public FunctionBuilderBase {

    std::shared_ptr<FunctionExpression> make(const std::string& name,
                                             const expression::Expressions& args) const override {
        return std::make_shared<FunctionType>(name, args);
    }

public:  // methods

    FunctionBuilder(const std::string& name, const char* help = 0) :
        FunctionBuilderBase(name, FunctionType::arity(), help ? help : FunctionType::help()) {}

    ~FunctionBuilder() override {}
};

//----------------------------------------------------------------------------------------------------------------------

#if 0

class FunctionFactoryBase {
protected:
	int    arity_;
	std::string name_;
	std::string help_;
    virtual std::shared_ptr<FunctionExpression> make(const std::string&,const expression::Expressions&) = 0;

public:
	//FunctionFactoryBase() : name_("FunctionFactory"), arity_(-1) {}
	FunctionFactoryBase(const std::string& name, int arity, const std::string& help);
    ~FunctionFactoryBase() override;

    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>, std::shared_ptr<SQLExpression>);
    std::shared_ptr<FunctionExpression> build(const std::string&, const expression::Expressions&);
};

class FunctionFactory : public FunctionFactoryBase {
public:
	static FunctionFactory& instance();
	FunctionFactory(); // : FunctionFactoryBase("FunctionFactory", -1) {}
    ~FunctionFactory() override {}

    typedef std::vector<std::pair<std::pair<std::string, int>, std::string> > FunctionInfo;

	FunctionInfo& functionsInfo();

private:
    std::shared_ptr<FunctionExpression> make(const std::string&,const expression::Expressions&) override { NOTIMP; return 0; }


    std::map<std::pair<std::string,int>, FunctionFactoryBase*> map_;
	FunctionInfo functionInfo_;
};

template<class T>
class FunctionMaker : public FunctionFactoryBase {
    std::shared_ptr<FunctionExpression> make(const std::string& name, const expression::Expressions& args) override {
        return std::make_shared<T>(name, args);
    }
public:
    FunctionMaker(const std::string& name, int arity, const std::string& help) :
        FunctionFactoryBase(name, arity, help) {}
};

#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression::function

#endif
