/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string_view>

#include "eckit/mpi/ParallelGroup.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/mpi/Parallel.h"

namespace eckit {
namespace mpi {


void MPICall(int code, std::string_view mpifunc, const eckit::CodeLocation& loc);
#define MPI_CALL(a) MPICall(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

ParallelGroup::~ParallelGroup() {
    if (valid_ && !Parallel::finalized()) {
        MPI_CALL(MPI_Group_free(&group_));
    }
}

ParallelGroup::ParallelGroup() :
    valid_{false} {}

ParallelGroup::ParallelGroup(MPI_Group group) :
    valid_{true},
    group_(group) {}

void ParallelGroup::print(std::ostream& os) const {
    os << "ParallelGroup()";
}

int ParallelGroup::group() const {
    return MPI_Group_c2f(group_);
}
int ParallelGroup::compare(const GroupContent& otherContent) const {
    return compare(dynamic_cast<const ParallelGroup&>(otherContent));
}

int ParallelGroup::compare(const ParallelGroup& other) const {
    int groupCompare;
    MPI_CALL(MPI_Group_compare(group_, other.group_, &groupCompare));
    switch (groupCompare) {
        case MPI_IDENT:
            return 0;
        case MPI_SIMILAR:
            return 1;
        default:
            return -1;
    }
}


GroupContent* ParallelGroup::difference(const GroupContent& otherContent) const {
    return difference(dynamic_cast<const ParallelGroup&>(otherContent));
};

ParallelGroup* ParallelGroup::difference(const ParallelGroup& other) const {
    MPI_Group newGroup;
    MPI_CALL(MPI_Group_difference(group_, other.group_, &newGroup));

    return new ParallelGroup(newGroup);
};


GroupContent* ParallelGroup::intersection(const GroupContent& otherContent) const {
    return intersection(dynamic_cast<const ParallelGroup&>(otherContent));
};

ParallelGroup* ParallelGroup::intersection(const ParallelGroup& other) const {
    MPI_Group newGroup;
    MPI_CALL(MPI_Group_intersection(group_, other.group_, &newGroup));

    return new ParallelGroup(newGroup);
};


GroupContent* ParallelGroup::union_group(const GroupContent& otherContent) const {
    return union_group(dynamic_cast<const ParallelGroup&>(otherContent));
};

ParallelGroup* ParallelGroup::union_group(const ParallelGroup& other) const {
    MPI_Group newGroup;
    MPI_CALL(MPI_Group_union(group_, other.group_, &newGroup));

    return new ParallelGroup(newGroup);
};

size_t ParallelGroup::size() const {
    int s;
    MPI_CALL(MPI_Group_size(group_, &s));

    return ((size_t)s);
};

int ParallelGroup::rank() const {
    int r;
    MPI_CALL(MPI_Group_rank(group_, &r));

    return r;
};


GroupContent* ParallelGroup::excl(const std::vector<int>& ranks) const {
    MPI_Group newGroup;
    MPI_CALL(MPI_Group_excl(group_, ranks.size(), ranks.data(), &newGroup));

    return new ParallelGroup(newGroup);
}

GroupContent* ParallelGroup::incl(const std::vector<int>& ranks) const {
    MPI_Group newGroup;
    MPI_CALL(MPI_Group_incl(group_, ranks.size(), ranks.data(), &newGroup));

    return new ParallelGroup(newGroup);
}

GroupContent* ParallelGroup::range_excl(const std::vector<std::array<int, 3>>& ranks) const {
    MPI_Group newGroup;
    int(*a)[3] = new int[ranks.size()][3];
    for (int i = 0; i < ranks.size(); ++i) {
        a[i][0] = ranks[i][0];
        a[i][1] = ranks[i][1];
        a[i][2] = ranks[i][2];
    }
    MPI_CALL(MPI_Group_range_excl(group_, ranks.size(), a, &newGroup));

    return new ParallelGroup(newGroup);
}

GroupContent* ParallelGroup::range_incl(const std::vector<std::array<int, 3>>& ranks) const {
    MPI_Group newGroup;
    int(*a)[3] = new int[ranks.size()][3];
    for (int i = 0; i < ranks.size(); ++i) {
        a[i][0] = ranks[i][0];
        a[i][1] = ranks[i][1];
        a[i][2] = ranks[i][2];
    }
    MPI_CALL(MPI_Group_range_incl(group_, ranks.size(), a, &newGroup));

    return new ParallelGroup(newGroup);
}

std::vector<int> ParallelGroup::translate_ranks_native(const std::vector<int>& ranks, const ParallelGroup& other) const {
    std::vector<int> newVector(ranks.size());
    MPI_CALL(MPI_Group_translate_ranks(group_, ranks.size(), ranks.data(), other.group_, newVector.data()));
    return newVector;
}

std::vector<int> ParallelGroup::translate_ranks_native(const std::vector<int>& ranks, const GroupContent& other) const {
    return translate_ranks_native(ranks, dynamic_cast<const ParallelGroup&>(other));
}

std::unordered_map<int, int> ParallelGroup::translate_ranks(const std::vector<int>& ranks, const GroupContent& other) const {
    std::unordered_map<int, int> map;
    std::vector<int> translated = translate_ranks_native(ranks, other);

    for (int r = 0; r < ranks.size(); ++r) {
        if (translated[r] != MPI_UNDEFINED) {
            map.emplace(ranks[r], translated[r]);
        }
    }

    return map;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit
