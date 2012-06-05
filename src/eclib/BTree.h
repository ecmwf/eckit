/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File BTree.h
// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef BTree_H
#define BTree_H

#include "eclib/Padded.h"
#include "eclib/PathName.h"

// IMPLEMENTS a B+Tree index
// TODO: Deletion
// TODO: Cache pages
//
// K and V needs to be PODs

#include "eclib/machine.h"
#include "eclib/PathName.h"
#include "eclib/Padded.h"

template<class K,class V, int S = 64*1024 >
class BTree {
public:

    typedef K key_type;
    typedef V value_type;

// -- Exceptions


// -- Contructors

    BTree(const PathName&);

// -- Destructor

    ~BTree(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods

    bool get(const K&, V&);
    bool set(const K&, const V&);

    bool remove(const K&);

    void dump(ostream& s = cout) const;
    void dump(ostream&, unsigned long page, int depth) const;


    void lock();
    void lockShared();
    void unlock();

    void flush();

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

protected:

// -- Methods

    void print(ostream&) const; // Change to virtual if base class

private:

// No copy allowed

    BTree(const BTree&);
    BTree& operator=(const BTree&);


    struct _Header {
    };

    struct NodeEntry {
        K    key_;
        unsigned long page_;

        bool operator<(const K& key) const {
            return key_ < key;
        }

        bool operator>(const K& key) const {
            return key_ > key;
        }

        bool operator==(const K& key) const {
            return key_ == key;
        }

        bool operator!=(const K& key) const {
            return key_ != key;
        }

        bool operator>=(const K& key) const {
            return key_ >= key;
        }

        bool operator<=(const K& key) const {
            return key_ <= key;
        }

        bool operator<(const NodeEntry& e) const {
            return key_ < e.key_;
        }

        bool operator>(const NodeEntry& e) const {
            return key_ > e.key_;
        }

        bool operator==(const NodeEntry& e) const {
            return key_ == e.key_;
        }

        bool operator!=(const NodeEntry& e) const {
            return key_ != e.key_;
        }

        bool operator>=(const NodeEntry& e) const {
            return key_ >= e.key_;
        }

        bool operator<=(const NodeEntry& e) const {
            return key_ <= e.key_;
        }
    };

    struct LeafEntry {
        K    key_;
        V    value_;

        bool operator<(const K& key) const {
            return key_ < key;
        }

        bool operator>(const K& key) const {
            return key_ > key;
        }

        bool operator==(const K& key) const {
            return key_ == key;
        }

        bool operator!=(const K& key) const {
            return key_ != key;
        }

        bool operator>=(const K& key) const {
            return key_ >= key;
        }

        bool operator<=(const K& key) const {
            return key_ <= key;
        }

        bool operator<(const NodeEntry& e) const {
            return key_ < e.key_;
        }

        bool operator>(const NodeEntry& e) const {
            return key_ > e.key_;
        }

        bool operator==(const NodeEntry& e) const {
            return key_ == e.key_;
        }

        bool operator!=(const NodeEntry& e) const {
            return key_ != e.key_;
        }

        bool operator>=(const NodeEntry& e) const {
            return key_ >= e.key_;
        }

        bool operator<=(const NodeEntry& e) const {
            return key_ <= e.key_;
        }
    };

    struct _Page {
        unsigned long  id_;
        unsigned long  count_;
        unsigned long  node_;
        unsigned long  left_;
	unsigned long  right_;

    };



    struct _LeafPage : public _Page {

        LeafEntry lentries_[1];
        void print(ostream& s) const ;
    };


    struct _NodePage : public _Page  {

        NodeEntry nentries_[1];
        void print(ostream& s) const ;
    };



    struct NodePage : Padded<_NodePage, S> {
    };

    struct LeafPage : Padded<_LeafPage, S> {
    };


    struct Page : Padded<_Page, S> {
        NodePage& nodePage() {
            return *(reinterpret_cast<NodePage*>(this));
        }
        LeafPage& leafPage() {
            return *(reinterpret_cast<LeafPage*>(this));
        }
        const NodePage& nodePage() const {
            return *(reinterpret_cast<const NodePage*>(this));
        }
        const LeafPage& leafPage() const {
            return *(reinterpret_cast<const LeafPage*>(this));
        }
        
        
        void print(ostream& s) const ;

        friend ostream& operator<<(ostream& s,const Page& p)
        {
            p.print(s);
            return s;
        }
    };



    // The -1 is here so we have one extra value for overflows
    static const size_t maxNodeEntries_ = (sizeof(NodePage) - sizeof(_Page)) / sizeof(NodeEntry) -1;
    static const size_t maxLeafEntries_ = (sizeof(LeafPage) - sizeof(_Page)) / sizeof(LeafEntry) -1;


    PathName path_;
    int fd_;
    
    bool cacheReads_;
    bool cacheWrites_;
    
    struct _PageInfo {
        time_t last_;
        Page* page_;
        bool dirty_;
        unsigned long long count_;

        _PageInfo(Page* page = 0):
            page_(page),
            count_(0),
            last_(time(0)),
            dirty_(false) {}
    };
 
    typedef map<unsigned long,_PageInfo> Cache;
    Cache cache_;

    void lockRange(off64_t start,off64_t len,int cmd,int type);
    bool search(unsigned long page, const K&, V&) const;

    void splitRoot();

    off64_t pageOffset(unsigned long) const;
    void savePage(const Page&);
    void loadPage(unsigned long,Page&) const;
    void newPage(Page&);

    void _savePage(const Page&);
    void _loadPage(unsigned long,Page&) const;
    void _newPage(Page&);

    bool insert(unsigned long page, const K& key, const V& value, vector<unsigned long>& path);
    bool store(unsigned long page, const K& key, const V& value, vector<unsigned long>& path);

    unsigned long next(const K&, const Page&) const;


// -- Friends

    friend ostream& operator<<(ostream& s,const BTree& p)
    {
        p.print(s);
        return s;
    }


};




#include "BTree.cc"
#endif
