/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_ParallelGroup_h
#define eckit_mpi_ParallelGroup_h

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include <mpi.h>

#include "eckit/mpi/Group.h"

namespace eckit {
namespace mpi {

//----------------------------------------------------------------------------------------------------------------------

class Parallel;

class ParallelGroup : public GroupContent {
public:
    virtual ~ParallelGroup() override;

private:  // constructor
    ParallelGroup();
    ParallelGroup(MPI_Group);

private:  // methods
    virtual void print(std::ostream&) const override;

    virtual int group() const override;

    virtual int compare(const GroupContent&) const override;

    int compare(const ParallelGroup&) const;

    virtual GroupContent* difference(const GroupContent&) const override;

    ParallelGroup* difference(const ParallelGroup&) const;

    virtual GroupContent* intersection(const GroupContent&) const override;

    ParallelGroup* intersection(const ParallelGroup&) const;

    virtual GroupContent* union_group(const GroupContent&) const override;

    ParallelGroup* union_group(const ParallelGroup&) const;

    virtual size_t size() const override;

    virtual int rank() const override;

    virtual GroupContent* excl(const std::vector<int>& ranks) const override;

    virtual GroupContent* incl(const std::vector<int>& ranks) const override;

    virtual GroupContent* range_excl(const std::vector<std::array<int, 3>>& ranks) const override;

    virtual GroupContent* range_incl(const std::vector<std::array<int, 3>>& ranks) const override;

    std::vector<int> translate_ranks_native(const std::vector<int>& ranks, const ParallelGroup& other) const;

    std::vector<int> translate_ranks_native(const std::vector<int>& ranks, const GroupContent& other) const;

    virtual std::unordered_map<int, int> translate_ranks(const std::vector<int>& ranks, const GroupContent& other) const override;

private:  // members
    friend class Parallel;

    bool valid_;
    MPI_Group group_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mpi
}  // namespace eckit

#endif
