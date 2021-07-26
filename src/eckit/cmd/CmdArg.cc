/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/CmdArg.h"
#include "eckit/utils/Translator.h"
#include "eckit/value/Value.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec CmdArg::classSpec_ = {
    &Streamable::classSpec(),
    "CmdArg",
};
Reanimator<CmdArg> CmdArg::reanimator_;

void CmdArg::encode(Stream& s) const {
    Streamable::encode(s);

    long size = args_.size();
    s << size;

    for (CmdMap::const_iterator i = args_.begin(); i != args_.end(); ++i) {
        std::string lhs = (*i).first;
        Value rhs       = (*i).second;
        s << lhs;
        s << rhs;
    }
}

CmdArg::CmdArg(Stream& s) :
    Streamable(s) {
    long size;
    s >> size;

    for (int i = 0; i < size; ++i) {
        std::string first;
        s >> first;

        Value second(s);
        args_[first] = second;
    }
}

CmdArg::CmdArg() {}

CmdArg::~CmdArg() {}

CmdArg::CmdArg(const CmdArg& other) :
    args_(other.args_) {}

CmdArg& CmdArg::operator=(const CmdArg& other) {
    args_ = other.args_;
    return *this;
}

const Value& CmdArg::get(const std::string& key) const {
    static Value empty;
    return get(key, empty);
}

const Value& CmdArg::get(const std::string& key, const Value& def) const {
    CmdMap::const_iterator j = args_.find(key);
    if (j == args_.end()) {
        return def;
    }
    else {
        return (*j).second;
    }
}

void CmdArg::set(const std::string& key, const Value& val) {
    args_[key] = val;
}

const Value& CmdArg::operator[](const std::string& s) const {
    return get(s);
}

const Value& CmdArg::operator[](const long l) const {
    return get(Translator<long, std::string>()(l));
}

Value& CmdArg::operator[](const std::string& s) {
    return args_[s];
}

Value& CmdArg::operator[](const long l) {
    return args_[Translator<long, std::string>()(l)];
}

void CmdArg::print(std::ostream& out) const {
    for (CmdMap::const_iterator i = args_.begin(); i != args_.end(); ++i)
        out << (*i).first << " = " << (*i).second << std::endl;
}

std::vector<std::string> CmdArg::args() const {
    std::vector<std::string> result;
    for (CmdMap::const_iterator i = args_.begin(); i != args_.end(); ++i)
        result.push_back((*i).first);
    return result;
}

void CmdArg::erase() {
    args_.erase(args_.begin(), args_.end());
}

void CmdArg::erase(long from, long to) {
    // Move the remaining, removing the trailing
    CmdMap::iterator j;
    long i = from;
    while ((j = args_.find(Translator<long, std::string>()(i))) != args_.end()) {
        CmdMap::iterator k = args_.find(Translator<long, std::string>()(to + 1 - from + i));
        if (k != args_.end())
            (*j).second = (*k).second;
        else
            args_.erase(j);
        ++i;
    }
}

void CmdArg::erase(const std::string& s) {
    CmdMap::iterator j = args_.find(s);
    if (j != args_.end())
        args_.erase(j);
}

void CmdArg::operator+=(const CmdArg& other) {
    Log::debug() << "Appending " << other << std::endl;
    Log::debug() << "To " << std::endl
                 << *this << std::endl;
    CmdArg tmp = other;

    // Find where we start adding
    long i = 0;
    CmdMap::iterator j;
    while ((j = args_.find(Translator<long, std::string>()(i))) != args_.end())
        ++i;

    // Dont copy arg[0]
    tmp.args_.erase(tmp.args_.find("0"));

    // Add the numerical arguments
    long k = 1;
    for (CmdMap::iterator l = tmp.args_.find(Translator<long, std::string>()(k)); l != tmp.args_.end();
         ++k, l = tmp.args_.find(Translator<long, std::string>()(k))) {
        (*this)[i + k - 1] = (*l).second;
        tmp.args_.erase(l);
    }

    // Add the rest
    for (CmdMap::iterator m = tmp.args_.begin(); m != tmp.args_.end(); ++m)
        args_[(*m).first] = (*m).second;
}

bool CmdArg::exists(const std::string& key) const {
    return (args_.find(key) != args_.end());
}

bool CmdArg::exists(long l) const {
    std::string key = Translator<long, std::string>()(l);
    return (args_.find(key) != args_.end());
}

size_t CmdArg::size() const {
    return args_.size();
}

const Value& CmdArg::operator()(const std::string& key, Value defaultval) {
    CmdMap::const_iterator j = args_.find(key);
    if (j == args_.end()) {
        set(key, defaultval);
        return args_[key];
    }
    else
        return (*j).second;
}

const Value& CmdArg::operator()(const long lkey, Value defaultval) {
    std::string key = Translator<long, std::string>()(lkey);
    return operator()(key, defaultval);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
