/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File BTree.h
// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef eckit_BTree_h
#define eckit_BTree_h

#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>

#include "eckit/container/BTree.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/PooledFileDescriptor.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/Padded.h"
#include "eckit/os/Stat.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class BTreeLock {
public:

    static void lockRange(int fd, off_t start, off_t len, int cmd, int type) {
        struct flock lock;

        lock.l_type   = type;
        lock.l_whence = SEEK_SET;
        lock.l_start  = start;
        lock.l_len    = len;

        SYSCALL(::fcntl(fd, cmd, &lock));
    }
};

class BTreeNoLock {
public:

    static void lockRange(int fd, off_t start, off_t len, int cmd, int type) {}
};

//----------------------------------------------------------------------------------------------------------------------

/// B+Tree index
///
/// @todo Deletion
/// @todo Cache pages
/// @invariant K and V needs to be PODs
/// @invariant S is the page size padding
/// @invariant L implements locking policy
///
template <class K, class V, int S, class L = BTreeNoLock>
class BTree : private NonCopyable {
public:

    typedef K key_type;
    typedef V value_type;
    typedef std::pair<K, V> result_type;

    // -- Contructors

    BTree(const PathName&, bool readOnly = false, off_t offset = 0);

    // -- Destructor

    ~BTree();

    // -- Methods

    bool get(const K&, V&);
    bool set(const K&, const V&);

    void preload();

    template <class T>
    void range(const K& key1, const K& key2, T& result);

    bool remove(const K&);

    void dump(std::ostream& s = std::cout) const;

    /// Counts the entries in the whole tree
    /// This is not an efficient call since it visits the whole tree. Use with care.
    size_t count() const;

    /// Counts the entries in a page of the tree
    size_t count(unsigned long page) const;

    void lock();
    void lockShared();
    void unlock();

    void flock();
    void funlock();

    void flush();
    void sync();

    const PathName& path() const { return path_; }

private:  // methods

    void dump(std::ostream&, unsigned long page, int depth) const;

    void print(std::ostream& o) const { dump(o); }

private:

    struct _Header {};

    struct NodeEntry {
        K key_;
        unsigned long page_;

        bool operator<(const K& key) const { return key_ < key; }

        bool operator>(const K& key) const { return key_ > key; }

        bool operator==(const K& key) const { return key_ == key; }

        bool operator!=(const K& key) const { return key_ != key; }

        bool operator>=(const K& key) const { return key_ >= key; }

        bool operator<=(const K& key) const { return key_ <= key; }

        bool operator<(const NodeEntry& e) const { return key_ < e.key_; }

        bool operator>(const NodeEntry& e) const { return key_ > e.key_; }

        bool operator==(const NodeEntry& e) const { return key_ == e.key_; }

        bool operator!=(const NodeEntry& e) const { return key_ != e.key_; }

        bool operator>=(const NodeEntry& e) const { return key_ >= e.key_; }

        bool operator<=(const NodeEntry& e) const { return key_ <= e.key_; }
    };

    struct LeafEntry {
        K key_;
        V value_;

        bool operator<(const K& key) const { return key_ < key; }

        bool operator>(const K& key) const { return key_ > key; }

        bool operator==(const K& key) const { return key_ == key; }

        bool operator!=(const K& key) const { return key_ != key; }

        bool operator>=(const K& key) const { return key_ >= key; }

        bool operator<=(const K& key) const { return key_ <= key; }

        bool operator<(const NodeEntry& e) const { return key_ < e.key_; }

        bool operator>(const NodeEntry& e) const { return key_ > e.key_; }

        bool operator==(const NodeEntry& e) const { return key_ == e.key_; }

        bool operator!=(const NodeEntry& e) const { return key_ != e.key_; }

        bool operator>=(const NodeEntry& e) const { return key_ >= e.key_; }

        bool operator<=(const NodeEntry& e) const { return key_ <= e.key_; }
    };

    struct _Page {
        unsigned long id_;
        unsigned long count_;
        unsigned long node_;
        unsigned long left_;
        unsigned long right_;
    };


    struct _LeafPage : public _Page {
        static const size_t SIZE = (S - sizeof(_Page)) / sizeof(LeafEntry);
        LeafEntry lentries_[SIZE];
        void print(std::ostream& s) const;
    };

    struct _NodePage : public _Page {
        static const size_t SIZE = (S - sizeof(_Page)) / sizeof(NodeEntry);
        NodeEntry nentries_[SIZE];
        void print(std::ostream& s) const;
    };


    struct NodePage : Padded<_NodePage, S> {};

    struct LeafPage : Padded<_LeafPage, S> {};


    struct Page : Padded<_Page, S> {
        NodePage& nodePage() { return *(reinterpret_cast<NodePage*>(this)); }
        LeafPage& leafPage() { return *(reinterpret_cast<LeafPage*>(this)); }
        const NodePage& nodePage() const { return *(reinterpret_cast<const NodePage*>(this)); }
        const LeafPage& leafPage() const { return *(reinterpret_cast<const LeafPage*>(this)); }


        void print(std::ostream& s) const;

        friend std::ostream& operator<<(std::ostream& s, const Page& p) {
            p.print(s);
            return s;
        }
    };

    static const size_t maxNodeEntries_ = _NodePage::SIZE;  // split at full page -- could be a percentage
    static const size_t maxLeafEntries_ = _LeafPage::SIZE;  // split at full page -- could be a percentage

    PathName path_;

    mutable PooledFileDescriptor file_;

    bool cacheReads_;
    bool cacheWrites_;
    bool readOnly_;
    off_t offset_;

    struct _PageInfo {
        Page* page_;
        unsigned long long count_;
        time_t last_;
        bool dirty_;

        _PageInfo(Page* page = 0) : page_(page), count_(0), last_(time(nullptr)), dirty_(false) {}
    };

    typedef std::map<unsigned long, _PageInfo> Cache;
    Cache cache_;

    void lockRange(off_t start, off_t len, int cmd, int type);
    bool search(unsigned long page, const K&, V&) const;

    template <class T>
    void search(unsigned long page, const K& key1, const K& key2, T& result);


    void splitRoot();

    off_t pageOffset(unsigned long) const;
    void savePage(const Page&);
    void loadPage(unsigned long, Page&) const;
    void newPage(Page&);

    void _savePage(const Page&);
    void _loadPage(unsigned long, Page&) const;
    void _newPage(Page&);

    bool insert(unsigned long page, const K& key, const V& value, std::vector<unsigned long>& path);
    bool store(unsigned long page, const K& key, const V& value, std::vector<unsigned long>& path);

    unsigned long next(const K&, const Page&) const;


    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const BTree& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "BTree.cc"

#endif
