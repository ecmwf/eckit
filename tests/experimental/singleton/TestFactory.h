/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TestFactory_h
#define TestFactory_h

#include <memory>
#include <string>

class TestBuilder {
public:
    virtual ~TestBuilder() {}
    virtual void build() = 0;
};

class TestFactory {
public:  // methods
    TestFactory();

    static TestFactory& instance();

    void regist(const std::string&, TestBuilder*);
    TestBuilder& get(const std::string&);

private:
    struct PImpl;
    std::unique_ptr<PImpl> pimpl_;
};

template <typename T>
class AutoRegistBuilder {
public:
    AutoRegistBuilder(const std::string& name) {
        std::cout << "auto register [" << name << "]" << std::endl;
        TestFactory::instance().regist(name, new T());
    }
};

#endif /* TestFactory_h */
