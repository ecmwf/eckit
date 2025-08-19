/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Tensor.h
/// @author Tiago Quintino
/// @date   May 2021

#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <functional>
#include <numeric>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/linalg/types.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/types/Types.h"

namespace eckit::linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Dense Tensor in column major storage order
/// This class is not meant to be accessed 1 element at a time, as its access isn't efficient enough in tight loops.
/// It is meant to be passed as a contiguous piece of data into linear algebra packages.
/// The operator() is only provided for inspection and testing.
///
/// Supports 2 memory layouts:
///   * Column-major (as in Fortran) where [fast idx] .... [slow idx] - also known as "left" layout
///   * Row-major (as in C) where [slow idx] .... [fast idx] - also known as "right" layout

template <typename S>
class Tensor {

public:  // class methods

    enum class Layout : int {  // specify underlying type to be "int" for interoperability
        Right    = 0,
        Left     = 1,
        RowMajor = Right,
        ColMajor = Left
    };

    static Size flatSize(const std::vector<Size>& shape) {
        return std::accumulate(std::begin(shape), std::end(shape), 1, std::multiplies<Size>());
    }

    static std::vector<Size> strides(Layout layout, const std::vector<Size>& shape) {
        std::vector<Size> s(shape.size());
        if (layout == Layout::ColMajor) {
            Size prod = 1;
            s[0]      = prod;
            for (int i = 1; i < s.size(); ++i) {
                prod *= shape[i - 1];
                s[i] = prod;
            }
        }
        else {
            Size tmp = 1;
            for (int i = shape.size() - 1; i >= 1; i--) {
                tmp *= shape[i];
                s[i - 1] = tmp;
            }
            s[shape.size() - 1] = 1;
        }
        return s;
    }

public:  // methods

    /// Default constructor (empty tensor)
    Tensor(Layout layout = Layout::ColMajor) :
        array_(0), size_(0), shape_(0), strides_(0), layout_(layout), own_(false) {}

    /// Construct tensor with given rows and columns (allocates memory, not initialised)
    Tensor(const std::vector<Size>& shape, Layout layout = Layout::ColMajor) :
        array_(nullptr), shape_(shape), strides_(strides(layout, shape)), layout_(layout), own_(true) {

        size_ = flatSize(shape_);
        ASSERT(size() > 0);
        array_ = new S[size_];
        ASSERT(array_);
    }

    /// Construct tensor from existing data (does NOT take ownership)
    Tensor(S* array, const std::vector<Size>& shape, Layout layout = Layout::ColMajor) :
        array_(array), strides_(strides(layout, shape)), layout_(layout), own_(false) {

        shape_ = shape;
        size_  = flatSize(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
    }

    /// Constructor from Stream
    Tensor(Stream& s) : array_(0), size_(0), shape_(0), own_(true) {
        Size shape_size;

        // layout
        int layoutAsInt;
        s >> layoutAsInt;
        layout_ = static_cast<Layout>(layoutAsInt);

        // shape
        s >> shape_size;
        shape_.resize(shape_size);
        for (auto& v : shape_) {
            s >> v;
        }
        resize(shape_);

        ASSERT(size() > 0);
        ASSERT(array_);

        // data
        s.readBlob(array_, size() * sizeof(S));
        strides_ = strides(layout_, shape_);
    }

    /// Copy constructor
    Tensor(const Tensor& other) :
        array_(new S[other.size()]),
        size_(other.size_),
        shape_(other.shape_),
        strides_(other.strides_),
        layout_(other.layout_),
        own_(true) {
        ASSERT(size() > 0);
        ASSERT(array_);
        ::memcpy(array_, other.array_, size() * sizeof(S));
    }

    /// Move constructor
    Tensor(Tensor&& other) noexcept {

        shape_   = std::move(other.shape_);
        strides_ = std::move(other.strides_);

        size_   = other.size_;
        layout_ = other.layout_;
        own_    = other.own_;

        array_ = other.array_;

        // nullify moved-from tensor
        other.array_ = nullptr;
        other.own_   = false;
        other.shape_.clear();
        other.strides_.clear();
        other.size_ = 0;
    }

    /// Destructor
    ~Tensor() {
        if (own_ && array_) {
            delete[] array_;
        }
    }

    /// Assignment
    /// Consistently retains ownership to avoid surprises in ownership behaviour.
    /// However, if a resize() triggers reallocation of memory, ownership will be reset.
    Tensor& operator=(const Tensor& other) {
        layout_ = other.layout_;
        resize(other.shape());
        // shape & strides already correct
        // ownership remains same
        ::memcpy(array_, other.array_, size() * sizeof(S));
        return *this;
    }

    /// Move assignment operator
    Tensor& operator=(Tensor&& other) noexcept {

        if (&other != this) {

            if (own_ && array_) {
                delete[] array_;
            }

            shape_   = std::move(other.shape_);
            strides_ = std::move(other.strides_);

            size_   = other.size_;
            layout_ = other.layout_;
            own_    = other.own_;

            array_ = other.array_;

            // nullify moved-from tensor
            other.array_ = nullptr;
            other.own_   = false;
            other.shape_.clear();
            other.strides_.clear();
            other.size_ = 0;
        }

        return *this;
    }

    /// Swap this tensor with another
    void swap(Tensor& other) {
        std::swap(array_, other.array_);
        std::swap(size_, other.size_);
        std::swap(shape_, other.shape_);
        std::swap(strides_, other.strides_);
        std::swap(layout_, other.layout_);
        std::swap(own_, other.own_);
    }

    /// Resize tensor to given a shape
    /// Invalidates data if shapes don't match, otherwise keeps data and simply reshapes
    void resize(const std::vector<Size>& shape) {
        if (this->size() != flatSize(shape)) {  // avoid reallocation if same size
            Tensor m(shape, layout_);
            swap(m);
        }
        else {  // optimise when we dont need to reallocate
            shape_   = shape;
            strides_ = strides(layout_, shape);
        }
    }

    /// Set data to zero
    void zero() {
        ASSERT(size() > 0);
        ASSERT(array_);
        ::memset(array_, 0, size() * sizeof(S));
    }

    /// Fill vector with given S
    void fill(S value) {
        for (Size i = 0; i < size(); ++i) {
            array_[i] = value;
        }
    }

    /// Serialise to a Stream
    /// This serialisation is not cross-platform
    void encode(Stream& s) const {
        s << static_cast<int>(layout_);
        s << shape_.size();
        for (auto v : shape_) {
            s << v;
        }
        s.writeBlob(array_, size() * sizeof(S));
    }

    /// @returns flatten size (= product of shape vector)
    Size size() const { return size_; }

    /// @returns shape
    std::vector<Size> shape() const { return shape_; }

    /// Access to linearised storage
    S& operator[](Size i) { return array_[i]; }
    const S& operator[](Size i) const { return array_[i]; }

    /// @returns modifiable view of the data
    S* data() { return array_; }
    /// @returns read-only view of the data
    const S* data() const { return array_; }

    /// @returns iterator to beginning of the data
    S* begin() { return array_; }
    /// @returns const iterator to beginning of the data
    const S* begin() const { return array_; }
    /// @returns iterator to end of the data
    S* end() { return array_ + size(); }
    /// @returns const iterator to end of the data
    const S* end() const { return array_ + size(); }

    void print(std::ostream& s) const {
        const char sep = ',';
        s << "Tensor(layout=" << static_cast<int>(layout_) << sep;
        s << "shape=[";
        for (int i = 0; i < shape_.size(); ++i) {
            s << shape_[i] << sep;
        }
        s << "],array=[";
        for (int i = 0; i < size(); ++i) {
            s << array_[i] << sep;
        }
        s << "])";
    }

    Layout layout() const { return layout_; }

    /// @brief Multidimensional index operator A(i,j,k,...)
    /// @pre  number of parameter must match shape size
    template <typename... Idx>
    S& operator()(Idx... idx) {
        return array_[index(idx...)];
    }

    /// @brief Multidimensional index operator A(i,j,k,...)
    /// @pre  number of parameter must match shape size
    template <typename... Idx>
    const S& operator()(Idx... idx) const {
        return array_[index(idx...)];
    }

    /// Transform a colMajor-layout tensor to rowMajor-layout
    Tensor transformColMajorToRowMajor() const {
        Tensor r(shape_);

        // COL-MAJOR to ROW-MAJOR
        std::vector<Size> strd_rev = strides(/*rowMajor*/ Layout::RowMajor, shape_);
        std::vector<Size> strd     = strides(/*colMajor*/ Layout::ColMajor, shape_);

        // main loop
        Size shape_size = shape_.size();
        std::vector<Size> col_major_indexes(shape_size);
        Size gidx_rm;
        for (int gidx_cm = 0; gidx_cm < size_; gidx_cm++) {

            // find the tensor indexes from the global index for a CM order
            for (int idx = 0; idx < shape_size - 1; idx++) {
                col_major_indexes[idx] = (gidx_cm % strd[idx + 1]) / strd[idx];
            }
            col_major_indexes[shape_size - 1] = gidx_cm / strd[shape_size - 1];

            // from the tensor indexes, work out the RM global index
            gidx_rm = 0;
            for (int idx = 0; idx < shape_size; idx++) {
                gidx_rm += col_major_indexes[idx] * strd_rev[idx];
            }

            // assign the corresponding tensor value
            *(r.data() + gidx_rm) = *(data() + gidx_cm);
        }

        // set layout to rowMajor
        r.layout_ = Layout::RowMajor;

        // strides are now colMajor-to-rowMajor
        r.strides_ = strd_rev;

        return r;
    }

    /// Transform a rowMajor-layout tensor to colMajor-layout
    Tensor transformRowMajorToColMajor() const {
        Tensor r(shape_);

        // ROW-MAJOR to COL-MAJOR
        std::vector<Size> strd_rev = strides(/*rowMajor*/ Layout::RowMajor, shape_);
        std::vector<Size> strd     = strides(/*colMajor*/ Layout::ColMajor, shape_);

        Size shape_size = shape_.size();
        std::vector<Size> row_major_indexes(shape_size);
        Size gidx_cm;
        for (int gidx_rm = 0; gidx_rm < size_; gidx_rm++) {

            // find the tensor indexes from the global index for a RM order
            row_major_indexes[0] = gidx_rm / strd_rev[0];
            for (int idx = 1; idx < shape_size; idx++) {
                row_major_indexes[idx] = gidx_rm % strd_rev[idx - 1] / strd_rev[idx];
            }

            // from the tensor indexes, work out the CM global index
            gidx_cm = 0;
            for (int idx = 0; idx < row_major_indexes.size(); idx++) {
                gidx_cm += row_major_indexes[idx] * strd[idx];
            }

            // assign the corresponding tensor value
            *(r.data() + gidx_cm) = *(data() + gidx_rm);
        }

        // set the layout as colMajor
        r.layout_ = Layout::ColMajor;

        // strides are now rowMajor-to-colMajor
        r.strides_ = strd;

        return r;
    }

private:  // methods

    /// compile time variadic template indexing calculation
    template <int Dim, typename Int, typename... Ints>
    constexpr Size index_part(Int idx, Ints... next_idx) const {
        return idx * strides_[Dim] + index_part<Dim + 1>(next_idx...);
    }

    /// compile time variadic template indexing calculation
    template <int Dim, typename Int>
    constexpr Size index_part(Int last_idx) const {
        return last_idx * strides_[Dim];
    }

    /// compile time variadic template indexing calculation
    template <typename... Ints>
    constexpr Size index(Ints... idx) const {
        return index_part<0>(idx...);
    }

protected:  // member variables

    S* array_;  ///< data

    Size size_;  ///< flattened size

    std::vector<Size> shape_;    ///< tensor shape is a vector of sizes per dimension
    std::vector<Size> strides_;  ///< tensor strides precomputed at construction

    Layout layout_;  ///< memory layout? (column-major equivalent to Fortran layout)
    bool own_;       ///< ownership
};

//----------------------------------------------------------------------------------------------------------------------

using TensorDouble = Tensor<double>;
using TensorFloat  = Tensor<float>;

//----------------------------------------------------------------------------------------------------------------------

template <typename S>
Stream& operator<<(Stream& s, const Tensor<S>& t) {
    t.encode(s);
    return s;
}

template <typename S>
std::ostream& operator<<(std::ostream& s, const Tensor<S>& t) {
    t.print(s);
    return s;
}

inline std::vector<size_t> shapify(int rank, int shape[]) {
    std::vector<size_t> result(rank);
    for (int i = 0; i < rank; ++i) {
        result[i] = shape[i];
    }
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
