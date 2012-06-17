/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/stat.h>
#include <fcntl.h>

#include "eclib/machine.h"
#include "eclib/AutoLock.h"
#include "eclib/BTree.h"
//===========================================================================================

template<class K,class V, int S>
void BTree<K, V, S>::Page::print(ostream& s) const
{
    s << ((this->node_) ? "NODE" : "LEAF" )
      <<  "_PAGE[id=" << this->id_ << ",count=" << this->count_ << "]";

    // For some strange reason "this" is required...
    if (this->node_) {
        this->nodePage().print(s);
    }
    else
    {
        this->leafPage().print(s);
    }
}

template<class K,class V, int S>
void BTree<K, V, S>::_LeafPage::print(ostream& s) const
{

    // For some strange reason "this" is required...
    s << "(";
    for (int i = 0; i < this->count_; i++)
    {
        s << lentries_[i].key_ << ":" << lentries_[i].value_ << ",";
    }
    s << ")";
    s << "[" << this->left_ << "<=>" << this->right_ << "]";

}

template<class K,class V, int S>
void BTree<K, V, S>::_NodePage::print(ostream& s) const
{
    // For some strange reason "this" is required...
    s << "{" << this->left_ << "!";
    for (int i = 0; i < this->count_; i++)
    {
        s << nentries_[i].key_ << "@" << nentries_[i].page_ << ",";
    }
    s << "}";
}


template<class K,class V, int S>
BTree<K,V,S>::BTree(const PathName& path):
    path_(path),
    fd_(-1),
    cacheReads_(true),
    cacheWrites_(true)
{
    SYSCALL(fd_ = ::open64(path.localPath(),O_RDWR|O_CREAT,0777));

    AutoLock<BTree<K,V,S> > lock(this);
    struct stat64 s;
    SYSCALL(fstat64(fd_, &s));


    if (s.st_size == 0) {

        // Add root page
        Page root;
        newPage(root);
        ASSERT(root.id_ == 1);
    }
    else
    {
        // TODO: Check header
    }

    compile_assert< (maxLeafEntries_>=2) >::check();
    compile_assert< (maxNodeEntries_>=2) >::check();

    compile_assert< (sizeof(Page) == sizeof(LeafPage)) >::check();
    compile_assert< (sizeof(Page) == sizeof(NodePage)) >::check();

    //cout << "::BTree : maxLeafEntries_=" << maxLeafEntries_ << ", maxNodeEntries_=" << maxNodeEntries_ << endl;
}


template<class K, class V, int S>
BTree<K,V,S>::~BTree()
{
    if (fd_>=0) {
        flush();
        SYSCALL(::close(fd_));
    }

    for(typename Cache::iterator j = cache_.begin(); j != cache_.end(); ++j)
        delete (*j).second.page_;
}

template<class K, class V, int S>
void BTree<K,V,S>::flush()
{
    for(typename Cache::iterator j = cache_.begin(); j != cache_.end(); ++j)
    {
        Log::info() << "BTree<K,V,S>::flush() " << path_ << " " << (*j).first << ", " << (*j).second.dirty_ << endl;
        if((*j).second.dirty_)
        {
            _savePage(*(*j).second.page_);
            (*j).second.dirty_ = false;
        }
    }
}

template<class K, class V, int S>
void BTree<K,V,S>::dump(ostream& s, unsigned long page, int depth) const
{

    Page p;
    loadPage(page, p);
    for (int i = 0; i < depth; i++)
        s << "   ";
    s << p << endl;
    if (p.node_)
    {
        dump(s, p.left_, depth+1);
        for (int i = 0; i < p.count_ ; i ++ )
            dump(s, p.nodePage().nentries_[i].page_, depth+1);
    }
}


template<class K, class V, int S>
void BTree<K,V,S>::dump(ostream& s) const
{
    AutoSharedLock<BTree<K,V,S> > lock(const_cast<BTree*>(this));
    s << "::BTree : maxLeafEntries_=" << maxLeafEntries_ << ", maxNodeEntries_=" << maxNodeEntries_ << endl;
    dump(s,1,0);
}


template<class K, class V, int S>
bool BTree<K,V,S>::set(const K& key, const V& value)
{
    AutoLock<BTree<K,V,S> > lock(this);
    //cout << "Set " << key << " -> " << value << endl;
    vector<unsigned long> path;
    return insert(1,key,value,path);
}


template<class K, class V, int S>
unsigned long BTree<K,V,S>::next(const K& key, const Page& p) const
{
    ASSERT(p.node_);

    const NodeEntry *begin = p.nodePage().nentries_;
    const NodeEntry *end   = begin + p.count_;

    ASSERT(begin != end);


    if (key < (*begin).key_)
    {
        //cout << "next " << key << " in " << p << " " <<  p.left_ << " (FIRST)" << endl;
        return p.left_;
    }

    const NodeEntry* e = std::lower_bound(begin, end, key);

    if (e == end || key < (*e).key_ ) {
        e--;
    }

    //cout << "next " << key << " -> " << (*e).key_ << ":" << (*e).page_ << endl;

    return (*e).page_;
}


template<class K, class V, int S>
bool BTree<K,V,S>::insert(unsigned long page, const K& key, const V& value, vector<unsigned long>& path)
{

    Page p;
    loadPage(page,p);

    //cout << "::VISIT " << p << endl;

    if (p.node_) {
        path.push_back(page);
        return insert(next(key, p), key, value, path);
    }

    LeafEntry *begin = p.leafPage().lentries_;
    LeafEntry *end   = begin + p.count_;

    LeafEntry* e = std::lower_bound(begin, end, key);

    //cout << "::store " << key << " in " << p << endl;
    //cout << "insert at at " << (e - p.lentries_) << endl;

    if ((e != end) && ((*e).key_ == key))
    {
        cout << "Page " << p.id_ << endl;
        cout << "Replace " << key << endl << (*e).value_ << endl << value << endl;
        (*e).value_ = value;
        savePage(p);
        return true;
    }

    // Assumes that K and V are PODs....
    size_t count = p.count_ - (e - begin);
    //cout << "Move count " << count << endl;
    memmove(e+1, e , count  * sizeof(LeafEntry));
    (*e).key_   = key;
    (*e).value_ = value;
    p.count_++;
    savePage(p);

    while ((p.node_ && (p.count_ == maxNodeEntries_)) || (!p.node_ && (p.count_ == maxLeafEntries_)) )
    {
        //cout << p << " needs spliting" << endl;
        if (p.id_ == 1) {
            splitRoot();
            return false;
        }
        else
        {
            int middle = p.count_ / 2;
            Page n;
            newPage(n);
            K k;
            // Same type
            n.node_ = p.node_;

            if (p.node_)
            {

                //cout << "SPLIT-NODE " << p << endl;

                for (size_t i = middle+1; i < p.count_ ; i ++ ) {
                    n.nodePage().nentries_[n.count_++] = p.nodePage().nentries_[i];
                }

                k = p.nodePage().nentries_[middle].key_;

                p.count_ -= n.count_;
                p.count_ --;

                n.left_ = p.nodePage().nentries_[middle].page_;

                // It's a node, Push-up


            }
            else
            {
                //cout << "SPLIT-LEAF " << p << endl;

                for (size_t i = middle; i < p.count_ ; i ++ ) {
                    n.leafPage().lentries_[n.count_++] = p.leafPage().lentries_[i];
                }

                p.count_ -= n.count_;

                // It's a leaf, Copy-up

                k = n.leafPage().lentries_[0].key_;


                if(p.right_) {
                    // TODO: do an I/O just for the linked list
                    Page r;
                    loadPage(p.right_,r);
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
            loadPage(page,p);

            ASSERT(p.node_);

            NodeEntry* begin = p.nodePage().nentries_;
            NodeEntry* end   = p.nodePage().nentries_ + p.count_;


            ASSERT(begin != end);

            NodeEntry* e = std::lower_bound(begin, end, k);

            // Node should not be there
            ASSERT(! ( e != end && (*e).key_ == k ) );

            size_t count = p.count_ - (e - begin);
            //cout << "Move node count " << count << endl;

            memmove(e+1, e , count  * sizeof(NodeEntry));

            (*e).key_   = k;
            (*e).page_  = n.id_;
            p.count_++;


            savePage(p);


        }
    }
    return false;
}


template<class K, class V, int S>
void BTree<K,V,S>::splitRoot()
{
    Page p;
    loadPage(1,p);

    Page pleft;
    Page pright;

    newPage(pleft);
    newPage(pright);

    //cout << "SPLIT ROOT " << p << endl;
    int middle = p.count_ / 2;

    K key;

    if (p.node_)
    {
        //dump();

        pleft.node_  = true;
        pright.node_ = true;


        pleft.left_ = p.left_;
        for (int i = 0; i < middle ; i ++ ) {
            pleft.nodePage().nentries_[pleft.count_++] = p.nodePage().nentries_[i];
        }

        pright.left_ = p.nodePage().nentries_[middle].page_;
        for (size_t i = middle+1; i < p.count_ ; i ++ ) {
            pright.nodePage().nentries_[pright.count_++] = p.nodePage().nentries_[i];
        }

        key = p.nodePage().nentries_[middle].key_;
    }
    else
    {

        pleft.node_  = false;
        pright.node_ = false;

        for (int i = 0; i < middle ; i ++ ) {
            pleft.leafPage().lentries_[pleft.count_++] = p.leafPage().lentries_[i];
        }


        for (size_t i = middle; i < p.count_ ; i ++ ) {
            pright.leafPage().lentries_[pright.count_++] = p.leafPage().lentries_[i];
        }

        key = pright.leafPage().lentries_[0].key_;

        pleft.right_ = pright.id_;
        pright.left_ = pleft.id_;
    }

    zero(p);
    p.id_    = 1;
    p.count_ = 1;
    p.node_  = true;

    p.left_  = pleft.id_;

    p.nodePage().nentries_[0].key_  = key;
    p.nodePage().nentries_[0].page_ = pright.id_;

    savePage(pright);
    savePage(pleft);
    savePage(p);
}


template<class K, class V, int S>
bool BTree<K,V,S>::get(const K& key, V& value)
{
    AutoSharedLock<BTree<K,V,S> > lock(this);

    V result;

    if (search(1, key, result))
    {
        //cout << "Found " << result << endl;
        value = result;
        return true;
    }

    //cout << "Not Found " << endl;
    return false;
}


template<class K, class V, int S>
bool BTree<K,V,S>::search(unsigned long page, const K& key, V& result) const
{
    Page p;
    loadPage(page, p);

    //cout << "Search " << key << ", Visit " << p << endl;

    if (p.node_) {
        return search(next(key,p), key, result);
    }

    const LeafEntry *begin = p.leafPage().lentries_;
    const LeafEntry *end   = begin + p.count_;

    const LeafEntry* e = std::lower_bound(begin, end, key);

    if ((e != end) && ((*e).key_ == key))
    {
        result = (*e).value_;
        return true;
    }

    return false;
}

template<class K, class V, int S>
void BTree<K,V,S>::range(const K& key1, const K& key2, vector<pair<K,V> >& result)
{
    AutoSharedLock<BTree<K,V,S> > lock(this);
    result.clear();
    search(1, key1, key2, result);
}


template<class K, class V, int S>
void BTree<K,V,S>::search(unsigned long page, const K& key1, const K& key2, vector<pair<K,V> >& result)
{
    Page p;
    loadPage(page, p);

    //cout << "Search " << key << ", Visit " << p << endl;

    if (p.node_) {
        return search(next(key1,p), key1, key2, result);
    }

    const LeafEntry *begin = p.leafPage().lentries_;
    const LeafEntry *end   = begin + p.count_;

    const LeafEntry* e = std::lower_bound(begin, end, key1);

    if (e == end)
        return;

    cout << p << endl;
    cout << "range " << (*e).key_ << " .... " << p.count_ << " " << (e - begin) << endl;
    cout << " key1 " << key1 << endl;
    cout << " key2 " << key2 << endl;

    while( !(key2 < (*e).key_) )
    {
        //cout << "range " << (*e).key_ << endl;
        result.push_back(pair<K,V>((*e).key_,(*e).value_));

        ++e;
        if(e == end)
        {
            if(p.right_) {
                loadPage(p.right_, p);
                ASSERT(!p.node_);
                e = p.leafPage().lentries_;
            }
            else
            {
                return;
            }
        }
    }
}


template<class K, class V, int S>
off64_t BTree<K,V,S>::pageOffset(unsigned long page) const
{
    ASSERT(page > 0); // Root page is 1. 0 in leaf marker
    return sizeof(Page) * off64_t(page-1);
}


template<class K, class V, int S>
void BTree<K,V,S>::_loadPage(unsigned long page, Page& p) const
{
    //cout << "Load " << page << endl;

    off64_t o = pageOffset(page);
    off64_t here;
    SYSCALL(here = lseek64(fd_,o,SEEK_SET));
    ASSERT(here == o);

    size_t len;
    SYSCALL(len = ::read(fd_,&p,sizeof(p)));
    ASSERT(len == sizeof(p));
    ASSERT(page == p.id_);
}

template<class K, class V, int S>
void BTree<K,V,S>::loadPage(unsigned long page, Page& p) const
{
    BTree<K,V,S>* self = const_cast<BTree<K,V,S>*>(this);

    typename Cache::iterator j = self->cache_.find(page);
    if(j != self->cache_.end())
    {
        // TODO: find someting better...
        memcpy(&p, (*j).second.page_, sizeof(Page));
        return;
    }


    _loadPage(page, p);

    if(cacheReads_)
    {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        self->cache_[p.id_] = _PageInfo(q);
    }

}

template<class K, class V, int S>
void BTree<K,V,S>::_savePage(const Page& p)
{
    //cout << "Save " << p << endl;

    off64_t o = pageOffset(p.id_);
    off64_t here;
    SYSCALL(here = lseek64(fd_,o,SEEK_SET));
    ASSERT(here == o);

    size_t len;
    SYSCALL(len = ::write(fd_,&p,sizeof(p)));
    ASSERT(len == sizeof(p));
}

template<class K, class V, int S>
void BTree<K,V,S>::savePage(const Page& p)
{
    BTree<K,V,S>* self = const_cast<BTree<K,V,S>*>(this);
    typename Cache::iterator j = self->cache_.find(p.id_);
    if(j != self->cache_.end())
    {
        // TODO: find someting better...
        memcpy((*j).second.page_, &p, sizeof(Page));
        (*j).second.dirty_ = true;
        (*j).second.count_++;
        return;
    }

    if(cacheWrites_)
    {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        self->cache_[p.id_] = _PageInfo(q);
        j = self->cache_.find(p.id_);
        (*j).second.dirty_ = true;
        (*j).second.count_++;
        return;
    }

    _savePage(p);
}

template<class K, class V,int S>
void BTree<K,V,S>::_newPage(Page& p)
{
    off64_t here;
    SYSCALL(here = lseek64(fd_,0,SEEK_END));

    unsigned long long page = here/sizeof(Page) + 1;

    //cout << "NEWPAGE " << page << endl;

    zero(p);
    p.id_ = (unsigned long)page;
    ASSERT(page == p.id_);
    ASSERT(pageOffset(page) == here);

    size_t len;
    SYSCALL(len = ::write(fd_,&p,sizeof(p))); // TODO: a sparse file....
    ASSERT(len == sizeof(p));

    //return p.id_;
}

template<class K, class V,int S>
void BTree<K,V,S>::newPage(Page& p)
{
    _newPage(p);

    if(cacheReads_ || cacheWrites_)
    {
        Page* q = new Page();
        memcpy(q, &p, sizeof(Page));
        cache_[p.id_] = _PageInfo(q);
    }
}


template<class K, class V, int S>
void BTree<K,V,S>::lockRange(off64_t start,off64_t len,int cmd,int type)
{

    struct flock64 lock;

    lock.l_type   = type;
    lock.l_whence = SEEK_SET;
    lock.l_start  = start;
    lock.l_len    = len;

    SYSCALL(::fcntl(fd_, cmd, &lock));
}


template<class K, class V,int S>
void BTree<K,V,S>::lockShared()
{
    lockRange(0,0,F_SETLKW64,F_RDLCK);
}


template<class K, class V,int S>
void BTree<K,V,S>::lock()
{
    lockRange(0,0,F_SETLKW64,F_WRLCK);
}


template<class K, class V,int S>
void BTree<K,V,S>::unlock()
{
    lockRange(0,0,F_SETLK64,F_UNLCK);
}
