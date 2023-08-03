/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/file.h>
#include <ostream>
#ifdef __linux__
#include <linux/errno.h>
#ifndef ENOTSUPP
#define ENOTSUPP 524
#endif
#endif

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FDataSync.h"
#include "eckit/memory/Zero.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::Page::print(std::ostream& s) const {
    s << ((this->node_) ? "NODE" : "LEAF") << "_PAGE[id=" << this->id_ << ",count=" << this->count_ << "]";

    // For some strange reason "this" is required...
    if (this->node_) {
        this->nodePage().print(s);
    }
    else {
        this->leafPage().print(s);
    }
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::_LeafPage::print(std::ostream& s) const {
    // For some strange reason "this" is required...
    s << "(";
    for (unsigned long i = 0; i < this->count_; i++) {
        s << lentries_[i].key_ << ":" << lentries_[i].value_ << ",";
    }
    s << ")";
    s << "[" << this->left_ << "<=>" << this->right_ << "]";
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::_NodePage::print(std::ostream& s) const {
    // For some strange reason "this" is required...
    s << "{" << this->left_ << "!";
    for (unsigned long i = 0; i < this->count_; i++) {
        s << nentries_[i].key_ << "@" << nentries_[i].page_ << ",";
    }
    s << "}";
}


template <class K, class V, int S, class L>
BTree<K, V, S, L>::BTree(const PathName& path, bool readOnly, off_t offset) :
    path_(path), file_(path, readOnly), cacheReads_(true), cacheWrites_(true), readOnly_(readOnly), offset_(offset) {
    file_.open();

    AutoLock<BTree<K, V, S, L> > lock(this);

    off_t here = file_.seekEnd();

    if (here <= offset_) {
        here = file_.seek(offset_);

        // Add root page
        Page root;
        newPage(root);
        ASSERT(root.id_ == 1);
    }
    else {
        // TODO: Check header
    }

    static_assert(maxLeafEntries_ > 3, "maxLeafEntries_ > 3");
    static_assert(maxNodeEntries_ > 3, "maxNodeEntries_ > 3");

    static_assert(sizeof(Page) == sizeof(LeafPage), "sizeof Page must be equal to sizeof LeafPage");
    static_assert(sizeof(Page) == sizeof(NodePage), "sizeof Page must be equal to sizeof NodePage");

    // std::cout << "::BTree : maxLeafEntries_=" << maxLeafEntries_ << ", maxNodeEntries_=" <<
    // maxNodeEntries_ << std::endl;
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::preload() {
    Page p;
    loadPage(1, p);

    while (p.right_) {
        loadPage(p.right_, p);
    }
}


template <class K, class V, int S, class L>
BTree<K, V, S, L>::~BTree() {
    if (file_.fileno() >= 0) {
        flush();
        file_.close();
    }

    for (typename Cache::iterator j = cache_.begin(); j != cache_.end(); ++j)
        delete (*j).second.page_;
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::flush() {
    for (typename Cache::iterator j = cache_.begin(); j != cache_.end(); ++j) {
        // Log::info() << "BTree<K,V,S,L>::flush() " << path_ << " " << (*j).first << ", " <<
        // (*j).second.dirty_ << std::endl;
        if ((*j).second.dirty_) {
            _savePage(*(*j).second.page_);
            (*j).second.dirty_ = false;
        }
    }
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::sync() {
    file_.sync();
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::dump(std::ostream& s, unsigned long page, int depth) const {
    Page p;
    loadPage(page, p);
    for (int i = 0; i < depth; i++)
        s << "   ";
    s << p << std::endl;
    if (p.node_) {
        dump(s, p.left_, depth + 1);
        for (unsigned long i = 0; i < p.count_; i++)
            dump(s, p.nodePage().nentries_[i].page_, depth + 1);
    }
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::dump(std::ostream& s) const {
    AutoSharedLock<BTree<K, V, S, L> > lock(const_cast<BTree*>(this));
    s << "::BTree : maxLeafEntries_=" << maxLeafEntries_ << ", maxNodeEntries_=" << maxNodeEntries_ << std::endl;
    dump(s, 1, 0);
}


template <class K, class V, int S, class L>
bool BTree<K, V, S, L>::set(const K& key, const V& value) {
    AutoLock<BTree<K, V, S, L> > lock(this);
    // std::cout << "Set " << key << " -> " << value << std::endl;
    std::vector<unsigned long> path;
    return insert(1, key, value, path);
}


template <class K, class V, int S, class L>
unsigned long BTree<K, V, S, L>::next(const K& key, const Page& p) const {
    ASSERT(p.node_);

    const NodeEntry* begin = p.nodePage().nentries_;
    const NodeEntry* end   = begin + p.count_;

    ASSERT(begin != end);

    if (key < (*begin).key_) {
        // std::cout << "next " << key << " in " << p << " " <<  p.left_ << " (FIRST)" << std::endl;
        return p.left_;
    }

    const NodeEntry* e = std::lower_bound(begin, end, key);

    if (e == end || key < (*e).key_) {
        e--;
    }

    // std::cout << "next " << key << " -> " << (*e).key_ << ":" << (*e).page_ << std::endl;

    return (*e).page_;
}


template <class K, class V, int S, class L>
bool BTree<K, V, S, L>::insert(unsigned long page, const K& key, const V& value, std::vector<unsigned long>& path) {
    Page p;
    loadPage(page, p);

    // std::cout << "::VISIT " << p << std::endl;

    if (p.node_) {
        path.push_back(page);
        return insert(next(key, p), key, value, path);
    }

    LeafEntry* begin = p.leafPage().lentries_;
    LeafEntry* end   = begin + p.count_;

    LeafEntry* e = std::lower_bound(begin, end, key);

    // std::cout << "::store " << key << " in " << p << std::endl;
    // std::cout << "insert at at " << (e - p.lentries_) << std::endl;

    if ((e != end) && ((*e).key_ == key)) {
        // std::cout << "Page " << p.id_ << " at pos " << e - begin << std::endl;
        // std::cout << "Replace " << key << std::endl << (*e).value_ << std::endl << value <<
        // std::endl;
        (*e).value_ = value;
        savePage(p);
        return true;
    }

    // Assumes that K and V are PODs....
    size_t count = p.count_ - (e - begin);
    // std::cout << "Move count " << count << std::endl;
    memmove(e + 1, e, count * sizeof(LeafEntry));
    (*e).key_   = key;
    (*e).value_ = value;
    p.count_++;
    savePage(p);

    while ((p.node_ && (p.count_ == maxNodeEntries_)) || (!p.node_ && (p.count_ == maxLeafEntries_))) {
        // std::cout << p << " needs spliting" << std::endl;

        if (p.id_ == 1) {
            splitRoot();
            return false;
        }
        else {
            int middle = p.count_ / 2;
            Page n;
            newPage(n);
            K k;
            // Same type
            n.node_ = p.node_;

            if (p.node_) {
                // std::cout << "SPLIT-NODE " << p << std::endl;

                for (size_t i = middle + 1; i < p.count_; ++i) {
                    n.nodePage().nentries_[n.count_++] = p.nodePage().nentries_[i];
                }

                ASSERT(n.count_ == p.count_ - middle - 1);

                k = p.nodePage().nentries_[middle].key_;

                p.count_ -= n.count_;
                p.count_--;

                n.left_ = p.nodePage().nentries_[middle].page_;

                // It's a node, Push-up
            }
            else {
                // std::cout << "SPLIT-LEAF " << p << std::endl;

                for (size_t i = middle; i < p.count_; ++i) {
                    n.leafPage().lentries_[n.count_++] = p.leafPage().lentries_[i];
                }

                ASSERT(n.count_ == p.count_ - middle);

                p.count_ -= n.count_;

                // It's a leaf, Copy-up

                k = n.leafPage().lentries_[0].key_;


                if (p.right_) {
                    // TODO: do an I/O just for the linked list
                    Page r;
                    loadPage(p.right_, r);
                    r.left_ = n.id_;
                    savePage(r);
                }

                n.right_ = p.right_;
                n.left_  = p.id_;
                p.right_ = n.id_;
            }

            savePage(n);
            savePage(p);

            page = path.back();
            path.pop_back();
            loadPage(page, p);

            ASSERT(p.node_);

            NodeEntry* begin = p.nodePage().nentries_;
            NodeEntry* end   = p.nodePage().nentries_ + p.count_;


            ASSERT(begin != end);

            NodeEntry* e = std::lower_bound(begin, end, k);

            // Node should not be there
            ASSERT(!(e != end && (*e).key_ == k));

            size_t count = p.count_ - (e - begin);
            // std::cout << "Move node count " << count << std::endl;

            memmove(e + 1, e, count * sizeof(NodeEntry));

            (*e).key_  = k;
            (*e).page_ = n.id_;
            p.count_++;


            savePage(p);
        }
    }
    return false;
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::splitRoot() {
    Page p;
    loadPage(1, p);

    Page pleft;
    Page pright;

    newPage(pleft);
    newPage(pright);

    // std::cout << "SPLIT ROOT " << p << std::endl;
    unsigned long middle = p.count_ / 2;

    K key;

    if (p.node_) {
        // dump();

        pleft.node_  = true;
        pright.node_ = true;


        pleft.left_ = p.left_;
        for (unsigned long i = 0; i < middle; i++) {
            pleft.nodePage().nentries_[pleft.count_++] = p.nodePage().nentries_[i];
        }

        pright.left_ = p.nodePage().nentries_[middle].page_;
        for (size_t i = middle + 1; i < p.count_; i++) {
            pright.nodePage().nentries_[pright.count_++] = p.nodePage().nentries_[i];
        }

        key = p.nodePage().nentries_[middle].key_;
    }
    else {
        pleft.node_  = false;
        pright.node_ = false;

        for (unsigned long i = 0; i < middle; ++i) {
            //		DEBUG_VAR( pleft.count_ );
            pleft.leafPage().lentries_[pleft.count_++] = p.leafPage().lentries_[i];
        }

        // Some version of Gcc (e.g. 4.8.1) optimize out the increment of this counter
        ASSERT(pleft.count_ == middle);


        for (size_t i = middle; i < p.count_; ++i) {
            pright.leafPage().lentries_[pright.count_++] = p.leafPage().lentries_[i];
        }


        // Some version of Gcc (e.g. 4.8.1) optimize out the increment of this counter
        ASSERT(pright.count_ == p.count_ - middle);

        key = pright.leafPage().lentries_[0].key_;

        pleft.right_ = pright.id_;
        pright.left_ = pleft.id_;
    }

    zero(p);
    p.id_    = 1;
    p.count_ = 1;
    p.node_  = true;

    p.left_ = pleft.id_;

    p.nodePage().nentries_[0].key_  = key;
    p.nodePage().nentries_[0].page_ = pright.id_;

    savePage(pright);
    savePage(pleft);
    savePage(p);

    //	cout << "LEFT\n" << pleft << endl;
    //	cout << "RIGHT\n" << pright << endl;
}


template <class K, class V, int S, class L>
bool BTree<K, V, S, L>::get(const K& key, V& value) {
    AutoSharedLock<BTree<K, V, S, L> > lock(this);

    V result;

    if (search(1, key, result)) {
        // std::cout << "Found " << result << std::endl;
        value = result;
        return true;
    }

    // std::cout << "Not Found " << std::endl;
    return false;
}


template <class K, class V, int S, class L>
bool BTree<K, V, S, L>::search(unsigned long page, const K& key, V& result) const {
    Page p;
    loadPage(page, p);

    // std::cout << "Search " << key << ", Visit " << p << std::endl;

    if (p.node_) {
        return search(next(key, p), key, result);
    }

    const LeafEntry* begin = p.leafPage().lentries_;
    const LeafEntry* end   = begin + p.count_;

    const LeafEntry* e = std::lower_bound(begin, end, key);

    if ((e != end) && ((*e).key_ == key)) {
        result = (*e).value_;
        return true;
    }

    return false;
}

template <class K, class V, int S, class L>
template <class T>
void BTree<K, V, S, L>::range(const K& key1, const K& key2, T& result) {
    AutoSharedLock<BTree<K, V, S, L> > lock(this);
    result.clear();
    search(1, key1, key2, result);
}

template <class K, class V, int S, class L>
bool BTree<K, V, S, L>::remove(const K&) {
    NOTIMP;
}

template <class K, class V, int S, class L>
template <class T>
void BTree<K, V, S, L>::search(unsigned long page, const K& key1, const K& key2, T& result) {
    Page p;
    loadPage(page, p);

    // std::cout << "Search " << key << ", Visit " << p << std::endl;

    if (p.node_) {
        return search(next(key1, p), key1, key2, result);
    }

    const LeafEntry* begin = p.leafPage().lentries_;
    const LeafEntry* end   = begin + p.count_;

    const LeafEntry* e = std::lower_bound(begin, end, key1);

    if (e == end)
        return;

    // while((*e).key_ < key1) { e++; if (e == end) return; }

    // std::cout << "range " << (*e).key_ << " .... " << p.count_ << " " << (e - begin) <<
    // std::endl;  std::cout << " key1 " << key1 << std::endl;  std::cout << " key2 " << key2 <<
    // std::endl;

    // std::cout << " begin " << (*begin).key_ << std::endl;
    if (p.count_) {
        // unused		const LeafEntry *last   = begin + p.count_ -1;
        // std::cout << " last "   << (*last).key_ << std::endl;
    }

    while (!(key2 < (*e).key_)) {
        // std::cout << "match " << p.id_ << " pos " << (e - begin) << " " << (*e).key_ <<
        // std::endl;
        result.push_back(result_type((*e).key_, (*e).value_));

        ++e;
        if (e == end) {
            if (p.right_) {
                loadPage(p.right_, p);
                ASSERT(!p.node_);
                e   = p.leafPage().lentries_;
                end = e + p.count_;
            }
            else {
                return;
            }
        }
    }
}


template <class K, class V, int S, class L>
off_t BTree<K, V, S, L>::pageOffset(unsigned long page) const {
    ASSERT(page > 0);  // Root page is 1. 0 is leaf marker
    return sizeof(Page) * off_t(page - 1) + offset_;
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::_loadPage(unsigned long page, Page& p) const {
    // std::cout << "Load " << page << std::endl;

    off_t o    = pageOffset(page);
    off_t here = file_.seek(o);
    ASSERT(here == o);

    int len = file_.read(&p, sizeof(p));
    ASSERT(len == sizeof(p));
    ASSERT(page == p.id_);
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::loadPage(unsigned long page, Page& p) const {
    BTree<K, V, S, L>* self = const_cast<BTree<K, V, S, L>*>(this);

    typename Cache::iterator j = self->cache_.find(page);
    if (j != self->cache_.end()) {
        // TODO: find someting better...
        memcpy(&p, (*j).second.page_, sizeof(Page));
        return;
    }


    _loadPage(page, p);

    if (cacheReads_) {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        self->cache_[p.id_] = _PageInfo(q);
    }
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::_savePage(const Page& p) {
    ASSERT(!readOnly_);
    // std::cout << "Save " << p << std::endl;

    off_t o = pageOffset(p.id_);
    off_t here;
    here = file_.seek(o);
    ASSERT(here == o);

    int len = file_.write(&p, sizeof(p));
    ASSERT(len == sizeof(p));
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::savePage(const Page& p) {
    BTree<K, V, S, L>* self    = const_cast<BTree<K, V, S, L>*>(this);
    typename Cache::iterator j = self->cache_.find(p.id_);
    if (j != self->cache_.end()) {
        // TODO: find someting better...
        memcpy((*j).second.page_, &p, sizeof(Page));
        (*j).second.dirty_ = true;
        (*j).second.count_++;
        return;
    }

    if (cacheWrites_) {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        self->cache_[p.id_] = _PageInfo(q);
        j                   = self->cache_.find(p.id_);
        (*j).second.dirty_  = true;
        (*j).second.count_++;
        return;
    }

    _savePage(p);
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::_newPage(Page& p) {
    ASSERT(!readOnly_);

    off_t here = file_.seekEnd();

    unsigned long long page = (here - offset_) / sizeof(Page) + 1;

    // std::cout << "NEWPAGE " << page << std::endl;

    zero(p);
    p.id_ = (unsigned long)page;
    ASSERT(page == p.id_);
    ASSERT(pageOffset(page) == here);

    int len = file_.write(&p, sizeof(p));  // TODO: a sparse file....
    ASSERT(len == sizeof(p));

    // return p.id_;
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::newPage(Page& p) {
    _newPage(p);

    if (cacheReads_ || cacheWrites_) {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        cache_[p.id_] = _PageInfo(q);
    }
}

template <class K, class V, int S, class L>
size_t BTree<K, V, S, L>::count() const {
    return count(1);
}

template <class K, class V, int S, class L>
size_t BTree<K, V, S, L>::count(unsigned long page) const {
    AutoSharedLock<BTree<K, V, S, L> > lock(const_cast<BTree*>(this));

    Page p;
    loadPage(page, p);

    size_t c = 0;

    if (p.node_) {
        c += this->count(p.left_);
        for (unsigned long i = 0; i < p.count_; i++)
            c += count(p.nodePage().nentries_[i].page_);
    }
    else  // leaf
    {
        c = p.count_;
    }

    return c;
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::lockShared() {
    L::lockRange(file_.fileno(), 0, 0, F_SETLKW, F_RDLCK);
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::lock() {
    L::lockRange(file_.fileno(), 0, 0, F_SETLKW, readOnly_ ? F_RDLCK : F_WRLCK);
}


template <class K, class V, int S, class L>
void BTree<K, V, S, L>::unlock() {
    L::lockRange(file_.fileno(), 0, 0, F_SETLK, F_UNLCK);
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::flock() {

    if (::flock(file_.fileno(), readOnly_ ? LOCK_EX : LOCK_SH) < 0) {
        bool skip = false;
#ifdef ENOTSUP
        if (errno == ENOTSUP) {
            skip = true;
        }
#endif
#ifdef ENOTSUPP
        if (errno == ENOTSUPP) {
            skip = true;
        }
#endif
#ifdef EOPNOTSUPP
        if (errno == EOPNOTSUPP) {
            skip = true;
        }
#endif
        if (!skip) {
            std::stringstream ss;
            ss << "Error " << Log::syserr << " locking " << path_;
            throw eckit::UserError(ss.str(), Here());
        }
    }
}

template <class K, class V, int S, class L>
void BTree<K, V, S, L>::funlock() {

    if (::flock(file_.fileno(), LOCK_UN) < 0) {
        bool skip = false;
#ifdef ENOTSUP
        if (errno == ENOTSUP) {
            skip = true;
        }
#endif
#ifdef ENOTSUPP
        if (errno == ENOTSUPP) {
            skip = true;
        }
#endif
#ifdef EOPNOTSUPP
        if (errno == EOPNOTSUPP) {
            skip = true;
        }
#endif
        if (!skip) {
            std::stringstream ss;
            ss << "Error " << Log::syserr << " unlocking " << path_;
            throw eckit::UserError(ss.str(), Here());
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
