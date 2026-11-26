// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Aug 2011

#ifndef eckit_EtcTable_h
#define eckit_EtcTable_h

#include <string>
#include <vector>

#include "eckit/thread/Mutex.h"


namespace eckit {


class EtcTable {
public:

    // -- Constructors

    EtcTable(const std::string&, int = 0, const std::string& = "etc");

    EtcTable(const EtcTable&)            = delete;
    EtcTable& operator=(const EtcTable&) = delete;
    EtcTable(EtcTable&&)                 = delete;
    EtcTable& operator=(EtcTable&&)      = delete;

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
