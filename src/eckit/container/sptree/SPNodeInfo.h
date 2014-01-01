/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNodeInfo_H
#define SPNodeInfo_H



namespace eckit {


template<class Traits>
class SPNode;

template<class Traits>
class SPTreeIterator;

template<class Traits>
class SPValue;

template<class Traits>
struct SPNodeInfo {

    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;
    typedef SPValue<Traits>             Value;
    typedef typename Alloc::Ptr   ID;

    typedef SPNode<Traits> Node;

    const Node* node_;
    double distance_;
    ID id_;

public:
    explicit SPNodeInfo(const Node* node = 0, ID id = 0, double distance = 0):
        node_(node), id_(id), distance_(distance) {}

    ID id() const {
        return id_;
    }

    bool operator<(const SPNodeInfo& other) const
    { return distance_ < other.distance_; }

    typedef std::vector<SPNodeInfo> NodeList;

    const Point& point() const { return node_->point(); }
    const Payload& payload() const { return node_->payload(); }
    const Value& value() const { return node_->value(); }

    double distance() const    { return distance_; }

    friend std::ostream& operator<<(std::ostream& s,const SPNodeInfo& p)
    {
        s << "[value=" << p.value() << ",distance=" << p.distance() << "]";
        return s;
    }
};


} // Name space


#endif
