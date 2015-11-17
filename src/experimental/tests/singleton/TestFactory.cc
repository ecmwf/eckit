/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <cassert>
#include <map>
#include <algorithm>
#include <exception>

#include "TestFactory.h"

struct TestFactory::PImpl {

    typedef std::map<std::string,TestBuilder*> Store;
    Store register_;

    void regist(const std::string& name, TestBuilder* builder)
    {
        std::cout << "registering builder [" << name << "]" << std::endl;
        Store::iterator itr = register_.find(name);
        if(itr != register_.end())
            throw std::bad_alloc();
        register_[name] = builder;
    }

    TestBuilder& get(const std::string& name)
    {
        std::cout << "get builder [" << name << "]" << std::endl;
        Store::iterator itr = register_.find(name);
        if(itr == register_.end())
            throw std::bad_alloc();
        assert( itr->second );
        return *(itr->second);
    }
};


//-----------------------------------------------------------------------------

TestFactory::TestFactory()
    :
    pimpl_( new TestFactory::PImpl() )
{
}

TestFactory& TestFactory::instance()
{
    // autolock mutex

    static TestFactory factory;
    return factory;
}

void TestFactory::regist(const std::string& name, TestBuilder* builder)
{
    pimpl_->regist(name,builder);
}

TestBuilder& TestFactory::get(const std::string& name)
{
    return pimpl_->get(name);
}
