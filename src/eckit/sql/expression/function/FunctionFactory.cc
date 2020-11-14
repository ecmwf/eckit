/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

//----------------------------------------------------------------------------------------------------------------------


FunctionFactory::FunctionFactory() {}

FunctionFactory::~FunctionFactory() {}

FunctionFactory& FunctionFactory::instance() {
    static FunctionFactory theInstance;
    return theInstance;
}

void FunctionFactory::enregister(const std::string& name, int arity, const FunctionBuilderBase* builder) {

    std::lock_guard<std::mutex> lock(m_);

    auto key = std::make_pair(name, arity);

    auto it = builders_.find(key);
    ASSERT(it == builders_.end());

    builders_[key] = builder;
}

void FunctionFactory::deregister(const std::string& name, int arity, const FunctionBuilderBase* builder) {

    std::lock_guard<std::mutex> lock(m_);

    auto it = builders_.find(std::make_pair(name, arity));

    ASSERT(it != builders_.end());
    ASSERT(it->second == builder);

    builders_.erase(it);
}

std::vector<FunctionFactory::FuncInfo> FunctionFactory::functionsInfo() {

    std::lock_guard<std::mutex> lock(m_);

    std::vector<FuncInfo> info;

    info.reserve(builders_.size());

    for (const auto& b : builders_) {
        ASSERT(b.first.second == b.second->arity());
        info.emplace_back(FuncInfo{b.first.first, b.second->arity(), b.second->help()});
    }

    return info;
}

std::shared_ptr<FunctionExpression> FunctionFactory::build(const std::string& name, const Expressions& args) const {

    std::lock_guard<std::mutex> lock(m_);

    // Do lookup for builder including the arity (-1 for "any")

    auto it = builders_.find(std::make_pair(name, int(args.size())));

    if (it == builders_.end()) {
        it = builders_.find(std::make_pair(name, -1));
    }

    if (it == builders_.end()) {
        throw UserError(name + ": function not defined", Here());
    }

    return it->second->make(name, args);
}

std::shared_ptr<FunctionExpression> FunctionFactory::build(const std::string& name,
                                                           std::shared_ptr<SQLExpression> arg1) {
    expression::Expressions args;
    args.push_back(arg1);
    return build(name, args);
}

std::shared_ptr<FunctionExpression> FunctionFactory::build(const std::string& name, std::shared_ptr<SQLExpression> arg1,
                                                           std::shared_ptr<SQLExpression> arg2) {
    expression::Expressions args;
    args.push_back(arg1);
    args.push_back(arg2);
    return build(name, args);
}

std::shared_ptr<FunctionExpression> FunctionFactory::build(const std::string& name, std::shared_ptr<SQLExpression> arg1,
                                                           std::shared_ptr<SQLExpression> arg2,
                                                           std::shared_ptr<SQLExpression> arg3) {
    expression::Expressions args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    return build(name, args);
}

FunctionBuilderBase::FunctionBuilderBase(const std::string& name, int arity, const char* help) :
    name_(name), arity_(arity), help_(help) {
    FunctionFactory::instance().enregister(name_, arity_, this);
}

FunctionBuilderBase::~FunctionBuilderBase() {
    FunctionFactory::instance().deregister(name_, arity_, this);
}

int FunctionBuilderBase::arity() const {
    return arity_;
}

std::string FunctionBuilderBase::help() const {
    return help_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
