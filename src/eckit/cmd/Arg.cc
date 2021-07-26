/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/Arg.h"

#include <iostream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ArgContent {
public:
    virtual ~ArgContent() {}
    virtual void print(std::ostream&, bool) const                                       = 0;
    virtual ArgContent* clone() const                                                   = 0;
    virtual void completion(const std::vector<std::string>&, std::vector<std::string>&) = 0;
    virtual void consume(std::vector<std::string>&)                                     = 0;
};

class ArgContentEmpty : public ArgContent {
    virtual void print(std::ostream& s, bool) const { s << ""; }
    virtual ArgContent* clone() const { return new ArgContentEmpty(); }

    virtual void completion(const std::vector<std::string>&, std::vector<std::string>&) {}
    virtual void consume(std::vector<std::string>&) {}
};

class ArgContentOption : public ArgContent {
    std::string name_;
    Arg::Type type_;

    virtual void print(std::ostream& s, bool) const { s << name_; }
    virtual ArgContent* clone() const { return new ArgContentOption(name_, type_); }

    virtual void completion(const std::vector<std::string>& s, std::vector<std::string>& r) {
        if (name_.find(s[0]) == 0) {
            r.push_back(name_);
        }
    }

    virtual void consume(std::vector<std::string>& v) {
        bool more = true;
        while (more) {
            more = false;
            for (std::vector<std::string>::iterator j = v.begin(); j != v.end(); ++j)
                if ((*j) == name_) {
                    more = true;
                    v.push_back("** marker **");
                    if ((*(j + 1))[0] != '-')
                        v.erase(j + 1);
                    v.erase(j);
                    break;
                }
        }
    }

public:
    ArgContentOption(const std::string& name, Arg::Type type) :
        name_(name), type_(type) {}
};

class ArgContentParam : public ArgContent {
    std::string name_;
    Arg::Type type_;

    virtual void print(std::ostream& s, bool) const { s << name_; }
    virtual ArgContent* clone() const { return new ArgContentParam(name_, type_); }

    virtual void completion(const std::vector<std::string>&, std::vector<std::string>&) {}

    virtual void consume(std::vector<std::string>&) {}

public:
    ArgContentParam(const std::string& name, Arg::Type type) :
        name_(name), type_(type) {}
};

class ArgContentOptional : public ArgContent {

    std::unique_ptr<ArgContent> content_;

    virtual void print(std::ostream& s, bool) const {
        s << "[";
        content_->print(s, false);
        s << "]";
    }
    virtual ArgContent* clone() const { return new ArgContentOptional(content_.get()); }

    virtual void completion(const std::vector<std::string>& s, std::vector<std::string>& r) {
        content_->completion(s, r);
    }

    virtual void consume(std::vector<std::string>& s) { content_->consume(s); }

public:
    ArgContentOptional(ArgContent* c) :
        content_(c->clone()) {}
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class ArgContentList : public ArgContent {
protected:
    std::vector<ArgContent*> list_;

    virtual ArgContent* clone() const;
    void push(ArgContent*);
    virtual void completion(const std::vector<std::string>& s, std::vector<std::string>& r);
    virtual void consume(std::vector<std::string>& s);

public:
    ArgContentList(ArgContent*, ArgContent*);
    ArgContentList(const std::vector<ArgContent*>&);
    ~ArgContentList();
};

template <class T>
void ArgContentList<T>::push(ArgContent* a) {
    ArgContentList* e = dynamic_cast<T*>(a);
    if (e) {
        for (size_t i = 0; i < e->list_.size(); i++)
            list_.push_back(e->list_[i]->clone());
    }
    else {
        list_.push_back(a->clone());
    }
}

template <class T>
ArgContentList<T>::ArgContentList(ArgContent* a1, ArgContent* a2) {
    push(a1);
    push(a2);
}

template <class T>
ArgContentList<T>::~ArgContentList() {
    for (size_t i = 0; i < list_.size(); i++) {
        delete list_[i];
        list_[i] = nullptr;
    }
}

template <class T>
ArgContentList<T>::ArgContentList(const std::vector<ArgContent*>& list) :
    list_(list) {
    for (size_t i = 0; i < list_.size(); i++)
        list_[i] = list_[i]->clone();
}

template <class T>
ArgContent* ArgContentList<T>::clone() const {
    return new T(list_);
}

template <class T>
void ArgContentList<T>::completion(const std::vector<std::string>& s, std::vector<std::string>& r) {
    for (size_t i = 0; i < list_.size(); i++)
        list_[i]->completion(s, r);
}

template <class T>
void ArgContentList<T>::consume(std::vector<std::string>& s) {
    for (size_t i = 0; i < list_.size(); i++)
        list_[i]->consume(s);
}

//----------------------------------------------------------------------------------------------------------------------

class ArgContentExclusive : public ArgContentList<ArgContentExclusive> {

    void print(std::ostream& s, bool bra) const {
        std::string p = "";
        if (bra)
            s << "(";
        for (size_t i = 0; i < list_.size(); i++) {
            s << p;
            list_[i]->print(s, true);
            p = " | ";
        }
        if (bra)
            s << ")";
    }

public:
    ArgContentExclusive(ArgContent* a1, ArgContent* a2) :
        ArgContentList<ArgContentExclusive>(a1, a2) {}
    ArgContentExclusive(const std::vector<ArgContent*>& a) :
        ArgContentList<ArgContentExclusive>(a) {}
};

//----------------------------------------------------------------------------------------------------------------------

class ArgContentInclusive : public ArgContentList<ArgContentInclusive> {

    void print(std::ostream& s, bool) const {
        std::string p = "";
        for (size_t i = 0; i < list_.size(); i++) {
            s << p;
            list_[i]->print(s, true);
            p = " ";
        }
    }

public:
    ArgContentInclusive(ArgContent* a1, ArgContent* a2) :
        ArgContentList<ArgContentInclusive>(a1, a2) {}
    ArgContentInclusive(const std::vector<ArgContent*>& a) :
        ArgContentList<ArgContentInclusive>(a) {}
};

//----------------------------------------------------------------------------------------------------------------------

Arg::Arg() :
    content_(new ArgContentEmpty()) {}

Arg::Arg(ArgContent* c) :
    content_(c) {}

Arg::Arg(const std::string& name, Type type) :
    content_(name[0] == '-' ? (ArgContent*)new ArgContentOption(name, type)
                            : (ArgContent*)new ArgContentParam(name, type)) {}

Arg::Arg(const Arg& other) :
    content_(other.content_->clone()) {}

Arg::~Arg() {}

void Arg::print(std::ostream& s) const {
    content_->print(s, false);
}

Arg Arg::operator~() {
    return Arg(new ArgContentOptional(content_.get()));
}

Arg operator|(const Arg& a1, const Arg& a2) {
    return Arg(new ArgContentExclusive(a1.content_.get(), a2.content_.get()));
}

Arg operator+(const Arg& a1, const Arg& a2) {
    return Arg(new ArgContentInclusive(a1.content_.get(), a2.content_.get()));
}

Arg& Arg::operator=(const Arg& other) {
    content_.reset(other.content_->clone());
    return *this;
}

std::vector<std::string> Arg::completion(std::vector<std::string>& s) {
    s.erase(s.begin());
    std::vector<std::string> result;
    content_->consume(s);
    content_->completion(s, result);
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
