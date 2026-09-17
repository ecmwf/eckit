// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef TestFactory_h
#define TestFactory_h

#include <iostream>
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
