/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Aug 2011

#ifndef eckit_EtcTable_h
#define eckit_EtcTable_h

#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {


class EtcTable : private NonCopyable {
public:

    // -- Contructors

    EtcTable(const std::string&, int = 0, const std::string& = "etc");

    // -- Destructor

    virtual ~EtcTable();

    const std::vector<std::string>& lookUp(const std::string&);
    std::vector<std::string> keys();
    std::vector<std::vector<std::string> > lines();

    bool reload();
    bool exists() const;


private:  // methods

    void load();

private:  // members

    time_t last_;
    std::string dir_;
    std::string name_;
    size_t size_;

    Mutex mutex_;

    std::vector<std::vector<std::string> > lines_;

private:  // methods

    virtual bool match(const std::string&, const std::vector<std::string>&) const = 0;
};


class EtcKeyTable : public EtcTable {
    bool match(const std::string& query, const std::vector<std::string>& line) const { return query == line[0]; }

public:

    EtcKeyTable(const std::string& name, int size = 0, const std::string& dir = "etc");
};


class EtcStartWithTable : public EtcTable {
    bool match(const std::string& query, const std::vector<std::string>& line) const {
        return query.find(line[0]) == 0;
    }

public:

    EtcStartWithTable(const std::string& name, int size = 0, const std::string& dir = "etc");
};


}  // namespace eckit

#endif
