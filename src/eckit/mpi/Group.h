/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Group_h
#define eckit_mpi_Group_h

#include <array>
#include <iosfwd>
#include <unordered_map>
#include <vector>

#include "eckit/memory/Counted.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class GroupContent : public Counted {
public:

    ~GroupContent() override;

    virtual void print(std::ostream&) const = 0;

    virtual int group() const = 0;

    virtual int compare(const GroupContent&) const = 0;

    virtual GroupContent* difference(const GroupContent&) const = 0;

    virtual GroupContent* intersection(const GroupContent&) const = 0;

    virtual GroupContent* union_group(const GroupContent&) const = 0;

    virtual size_t size() const = 0;

    virtual int rank() const = 0;

    virtual GroupContent* excl(const std::vector<int>& ranks) const = 0;

    virtual GroupContent* incl(const std::vector<int>& ranks) const = 0;

    virtual GroupContent* range_excl(const std::vector<std::array<int, 3>>& ranks) const = 0;

    virtual GroupContent* range_incl(const std::vector<std::array<int, 3>>& ranks) const = 0;

    virtual std::unordered_map<int, int> translate_ranks(const std::vector<int>&, const GroupContent&) const = 0;

private:

    friend class Group;
};

//----------------------------------------------------------------------------------------------------------------------

/// Group by construction has always a valid content_
/// @invariant content_ is not null

class Group {

public:  // methods

    /// Null request constructor
    Group();
    /// Group constructor from the Group() integer
    /// Use only for interfacing with Fortran
    Group(int);
    /// Constructor
    Group(GroupContent*);

    ~Group();

    Group(const Group&);

    Group& operator=(const Group&);

    template <class T>
    T& as() {
        return dynamic_cast<T&>(*content_);
    }

    template <class T>
    const T& as() const {
        return dynamic_cast<const T&>(*content_);
    }

    /// Returns this request interpreted as a int by the underlying implementation
    /// Use only for interfacing with Fortran
    int group() const;

    // Compares a group.
    //
    // Returns
    //  * an negative integer (< 0): if the groups are not equal (MPI_UNQUAL)
    //  * zero (= 0): if the groups are identical (MPI_IDENT), i.e. members are in exactly the same order in both groups
    //  * a positive integer (> 0): if the groups are similar (MPI_SIMILAR), i.e. both groups contain the same members
    //  but in different order
    int compare(const Group&) const;

    Group difference(const Group&) const;

    Group intersection(const Group&) const;

    // Can not name the function union because it is a reserved word
    Group union_group(const Group&) const;

    size_t size() const;

    // Returns the rank of the calling process in the given group.
    int rank() const;

    Group excl(const std::vector<int>& ranks) const;

    Group incl(const std::vector<int>& ranks) const;

    Group range_excl(const std::vector<std::array<int, 3>>& ranks) const;

    Group range_incl(const std::vector<std::array<int, 3>>& ranks) const;

    std::unordered_map<int, int> translate_ranks(const std::vector<int>&, const Group&) const;

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Group& o) {
        o.print(s);
        return s;
    }

private:  // members

    GroupContent* content_;

    friend class Parallel;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
