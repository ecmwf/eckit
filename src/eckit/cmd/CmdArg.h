/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_cmd_CmdArg_H
#define eckit_cmd_CmdArg_H

#include "eckit/config/Resource.h"
#include "eckit/serialisation/Streamable.h"
#include "eckit/value/Value.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CmdArg : public eckit::Streamable {
public:

    // -- Contructors

    CmdArg();
    CmdArg(eckit::Stream&);

    // -- Destructor

    ~CmdArg();

    // -- Copy

    CmdArg(const CmdArg&);
    CmdArg& operator=(const CmdArg&);

    // -- Operators

    void operator+=(const CmdArg&);

    // -- Methods

    const eckit::Value& operator[](const std::string&) const;
    const eckit::Value& operator[](const long) const;
    eckit::Value& operator[](const std::string&);
    eckit::Value& operator[](const long);

    const eckit::Value& operator()(const std::string&, eckit::Value);
    const eckit::Value& operator()(const long, eckit::Value);

    size_t size() const;

    void erase();
    void erase(long, long);
    void erase(const std::string&);

    bool exists(const std::string&) const;
    bool exists(long) const;

    std::vector<std::string> args() const;

    // From Streamble

    void encode(eckit::Stream&) const override;
    const eckit::ReanimatorBase& reanimator() const override { return reanimator_; }

    // access the value or its default
    const eckit::Value& get(const std::string&, const eckit::Value&) const;

    template <typename T>
    const eckit::Value get(const std::string& name, const std::string& resource, const T& def) const {
        T v = eckit::Resource<T>(resource, def);
        return get(name, v);
    }

protected:

    // -- Methods

    void print(std::ostream&) const;

    // Because the use of 'Accessor'
    const eckit::Value& get(const std::string&) const;
    void set(const std::string&, const eckit::Value&);

private:

    // -- Members

    typedef std::map<std::string, eckit::Value, std::less<std::string> > CmdMap;

    CmdMap args_;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const CmdArg& p) {
        p.print(s);
        return s;
    }

    // These have no sense. Only to let the STL have a Stack of CmdArg
    friend bool operator<(const CmdArg& lhs, const CmdArg& rhs) { return lhs.args_ < rhs.args_; }
    friend bool operator==(const CmdArg& lhs, const CmdArg& rhs) { return lhs.args_ == rhs.args_; }

    // -- Class members

    static eckit::ClassSpec classSpec_;
    static eckit::Reanimator<CmdArg> reanimator_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
