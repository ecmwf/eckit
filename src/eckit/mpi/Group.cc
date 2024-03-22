/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/mpi/Group.h"

#include "eckit/mpi/Comm.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class NullGroupContent : public GroupContent {
public:
    virtual ~NullGroupContent() {}

    virtual void print(std::ostream& os) const { os << "NullGroup()"; }

    virtual int group() const { return -1; }

    virtual int compare(const GroupContent&) const { return -1; }

    virtual NullGroupContent* difference(const GroupContent&) const { return new NullGroupContent(); }

    virtual NullGroupContent* intersection(const GroupContent&) const { return new NullGroupContent(); }

    virtual NullGroupContent* union_group(const GroupContent&) const { return new NullGroupContent(); }

    virtual size_t size() const { return 0; }

    virtual int rank() const { return -1; }

    virtual NullGroupContent* excl(const std::vector<int>& ranks) const { return new NullGroupContent(); };

    virtual NullGroupContent* incl(const std::vector<int>& ranks) const { return new NullGroupContent(); };

    virtual NullGroupContent* range_excl(const std::vector<std::array<int, 3>>& ranks) const { return new NullGroupContent(); };

    virtual NullGroupContent* range_incl(const std::vector<std::array<int, 3>>& ranks) const { return new NullGroupContent(); };

    virtual std::unordered_map<int, int> translate_ranks(const std::vector<int>&, const GroupContent&) const { return std::unordered_map<int, int>{}; };
};

//----------------------------------------------------------------------------------------------------------------------

Group::Group() :
    content_(new NullGroupContent()) {
    content_->attach();
}

Group::Group(int group) :
    content_(0) {
    *this = eckit::mpi::comm().group(group);
}

Group::Group(GroupContent* p) :
    content_(p) {
    content_->attach();
}

Group::~Group() {
    content_->detach();
}

Group::Group(const Group& s) :
    content_(s.content_) {
    content_->attach();
}

Group& Group::operator=(const Group& s) {
    if (content_) {
        content_->detach();
    }
    content_ = s.content_;
    content_->attach();
    return *this;
}

int Group::group() const {
    return content_->group();
}

int Group::compare(const Group& other) const {
    return content_->compare(*other.content_);
}

Group Group::difference(const Group& other) const {
    return Group(content_->difference(*other.content_));
}

Group Group::intersection(const Group& other) const {
    return Group(content_->intersection(*other.content_));
}

Group Group::union_group(const Group& other) const {
    return Group(content_->union_group(*other.content_));
}

size_t Group::size() const {
    return content_->size();
}

int Group::rank() const {
    return content_->rank();
}

Group Group::excl(const std::vector<int>& ranks) const {
    return Group(content_->excl(ranks));
};

Group Group::incl(const std::vector<int>& ranks) const {
    return Group(content_->incl(ranks));
};

Group Group::range_excl(const std::vector<std::array<int, 3>>& ranks) const {
    return Group(content_->range_excl(ranks));
};

Group Group::range_incl(const std::vector<std::array<int, 3>>& ranks) const {
    return Group(content_->range_incl(ranks));
};

std::unordered_map<int, int> Group::translate_ranks(const std::vector<int>& ranks, const Group& other) const {
    return content_->translate_ranks(ranks, *other.content_);
};

void Group::print(std::ostream& out) const {
    content_->print(out);
}

GroupContent::~GroupContent() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi
