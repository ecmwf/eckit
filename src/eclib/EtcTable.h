/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File EtcTable.h
// Baudouin Raoult - (c) ECMWF Aug 11

#ifndef EtcTable_H
#define EtcTable_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"
#include "eclib/Mutex.h"


class EtcTable : public NonCopyable<EtcTable> {
public:

// -- Contructors

    EtcTable(const string&, int = 0, const string& = "etc");

// -- Destructor

    virtual ~EtcTable();

	const vector<string>& lookUp(const string&);
    vector<string> keys();

private: // methods

    void load();

private: // members

    time_t last_;
    string name_;
    string dir_;
    int  size_;

    Mutex mutex_;

    vector<vector<string> > lines_;

private: // methods

    virtual bool match(const string&, const vector<string>&) const  = 0;

};

//-----------------------------------------------------------------------------

class EtcKeyTable : public EtcTable {
    bool match(const string& query, const vector<string>& line) const { return query == line[0]; }
public:
    EtcKeyTable(const string& name, int size = 0, const string& dir= "etc"):
        EtcTable(name, size, dir) {}
};

//-----------------------------------------------------------------------------

class EtcStartWithTable : public EtcTable {
    bool match(const string& query, const vector<string>& line) const { return query.find(line[0]) == 0; }
public:
    EtcStartWithTable(const string& name, int size = 0, const string& dir= "etc"):
        EtcTable(name, size, dir) {}
};

#endif
