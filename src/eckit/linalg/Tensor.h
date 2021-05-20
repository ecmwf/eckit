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
#include <cstring>
#include <functional>
#include <numeric>
#include <vector>

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/linalg/types.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Dense Tensor in column major storage order
/// This class is not meant to be accessed 1 element at a time, as its access isn't efficient enough in tight loops.
/// It is meant to be passed as a contiguous piece of data into linear algebra packages.
/// The operator() is only provided for inspection and testing.
///
/// Supports 2 memory layouts:
///   * Right layout equivalent to matrix column-major (as in Fortran) where [fast idx] .... [slow idx]
///   * Left layout equivalent to matrix row-major (as in C) where [slow idx] .... [fast idx]

template<typename S>
class Tensor {
public:  // types
    typedef eckit::linalg::Size Size;

public: // class methods
    
    static Size flatten(const std::vector<Size>& shape) {
        return std::accumulate(std::begin(shape), std::end(shape), 1, std::multiplies<Size>());
    }

    static std::vector<Size> strides(const std::vector<Size>& shape) {
        std::vector<Size> s(shape.size());
        Size prod  = 1;
        s[0] = prod;
        for (int i = 1; i < s.size(); ++i) {
            prod *= shape[i-1];
            s[i] = prod;
        }
        // std::cout << "shape : " << shape << "\nstrides : " << s << std::endl;
        return s;
    }

    static std::vector<Size> strides_rev(const std::vector<Size>& shape) {

        // shape cumulative reverse
        std::vector<Size> shape_cumul_reverse(shape.size());
        Size tmp=1;
        for (int i=shape.size()-1; i>=1; i--){
            tmp *= shape[i];
            shape_cumul_reverse[i-1] = tmp;
        }
        shape_cumul_reverse[shape.size()-1] = 1;

        return shape_cumul_reverse;
    }

public:  // methods
    
    /// Default constructor (empty tensor)
    Tensor() : array_(0), size_(0), shape_(0),
               strides_(0), strides_rev_(0),
               right_(true), own_(false) {}

    /// Construct tensor with given rows and columns (allocates memory, not initialised)
    Tensor(const std::vector<Size>& shape) : array_(nullptr), shape_(shape),
                                             strides_(strides(shape)), strides_rev_(strides_rev(shape)),
                                             right_(true), own_(true) {
        size_ = flatten(shape_);
        ASSERT(size() > 0);
        array_ = new S[size_];
        ASSERT(array_);
    }

    /// Construct tensor from existing data (does NOT take ownership)
    Tensor(const S* array, const std::vector<Size>& shape) :
        array_(const_cast<S*>(array)),
        strides_(strides(shape)), strides_rev_(strides_rev(shape)),
        right_(true), own_(false) {

        shape_ = shape;
        size_  = flatten(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
    }

    /// Constructor from Stream
    Tensor(Stream& s) : array_(0), size_(0), shape_(0), right_(true), own_(true) {
        Size shape_size;
        s >> shape_size;
        shape_.resize(shape_size);
        for (auto& v : shape_)
            s >> v;
        resize(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
        s.readBlob(array_, size() * sizeof(S));
        strides_ = strides(shape_);
        strides_rev_ = strides_rev(shape_);
    }

    /// Copy constructor
    Tensor(const Tensor& other) : array_(new S[other.size()]), size_(other.size_), shape_(other.shape_),
                                  strides_(other.strides_), strides_rev_(other.strides_rev_),
                                  right_(true), own_(true) {
        ASSERT(size() > 0);
        ASSERT(array_);
        ::memcpy(array_, other.array_, size() * sizeof(S));
    }

    /// Destructor
    ~Tensor() {
        if (own_) {
            delete[] array_;
        }
    }

    /// Assignment
    /// Not optimized for if size() == other.size(), as using copy constructor
    /// consistently retains ownership to avoid surprises in ownership behaviour.
    Tensor& operator=(const Tensor& other) {
        Tensor copy(other);
        swap(copy);
        return *this;
    }

    void toLeftLayout() {
        if (not right_)
            return;
        move(transformRigthToLeftLayout());
    }

    void toRightLayout() {
        if (right_)
            return;
        move(transformLeftToRightLayout());
    }

    /// Swap this tensor with another
    void swap(Tensor& other) {
        std::swap(array_, other.array_);
        std::swap(size_, other.size_);
        std::swap(shape_, other.shape_);
        std::swap(strides_, other.strides_);
        std::swap(right_, other.right_);
        std::swap(own_, other.own_);
    }

    /// Resize tensor to given a shape
    /// Invalidates data if shapes don't match, otherwise keeps data and simply reshapes
    void resize(const std::vector<Size>& shape) {
        if (this->size() != flatten(shape)) {  // avoid reallocation if same size
            Tensor m(shape);
            swap(m);
        }
        else {
            shape_ = shape;
            strides_ = strides(shape_);
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
        s << shape_.size();
        for (auto v : shape_)
            s << v;
        s.writeBlob(const_cast<S*>(array_), size() * sizeof(S));
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
        s << "Tensor(shape=[";
        for (int i = 0; i < shape_.size(); ++i) {
            s << shape_[i] << sep;
        }
        s << "],array=[";
        for (int i = 0; i < size(); ++i) {
            s << array_[i] << sep;
        }
        s << "])";
    }

    bool isRight() const {return right_;}

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

private: // methods

    /// compile time variadic template indexing calculation
    template <int Dim, typename Int, typename... Ints>
    constexpr Size index_part(Int idx, Ints... next_idx) const {
        Size strd = (right_)? strides_[Dim] : strides_rev_[Dim];
        return idx * strd + index_part<Dim + 1>(next_idx...);

    }

    /// compile time variadic template indexing calculation
    template <int Dim, typename Int>
    constexpr Size index_part(Int last_idx) const {
        Size strd = (right_)? strides_[Dim] : strides_rev_[Dim];
        return last_idx * strd;

    }

    /// compile time variadic template indexing calculation
    template <typename... Ints>
    constexpr Size index(Ints... idx) const {
        return index_part<0>(idx...);
    }

    void move(Tensor&& other) { swap(other); }

    Tensor transformRigthToLeftLayout() const {
        Tensor r(shape_);

        // COL-MAJOR to ROW-MAJOR

        // main loop
        Size shape_size = shape_.size();
        std::vector<Size> col_major_indexes(shape_size);
        Size gidx_rm;
        for (int gidx_cm=0; gidx_cm<size_; gidx_cm++){

            // find the tensor indexes from the global index for a CM order
            for (int idx=0; idx<shape_size-1; idx++){
                col_major_indexes[idx] = (gidx_cm % strides_[idx+1])/strides_[idx];
            }
            col_major_indexes[shape_size-1] = gidx_cm / strides_[shape_size-1];

            // from the tensor indexes, work out the RM global index
            gidx_rm = 0;
            for(int idx=0; idx<shape_size; idx++){
                gidx_rm += col_major_indexes[idx] * strides_rev_[idx];
            }

            // assign the corresponding tensor value
            *(r.data()+gidx_rm) = *(data()+gidx_cm);
        }

        // set the right flag to false
        r.right_ = false;

        return r;
    }

    Tensor transformLeftToRightLayout() const {
        Tensor r(shape_);

        // ROW-MAJOR to COL-MAJOR

        Size shape_size = shape_.size();
        std::vector<Size> row_major_indexes(shape_size);
        Size gidx_cm;
        for (int gidx_rm=0; gidx_rm<size_; gidx_rm++){

            // find the tensor indexes from the global index for a RM order
            row_major_indexes[0] = gidx_rm / strides_rev_[0];
            for (int idx=1; idx<shape_size; idx++){
                row_major_indexes[idx] = gidx_rm % strides_rev_[idx-1] / strides_rev_[idx];
            }

            // from the tensor indexes, work out the CM global index
            gidx_cm = 0;
            for (int idx=0; idx<row_major_indexes.size(); idx++){
                gidx_cm += row_major_indexes[idx] * strides_[idx];
            }

            // assign the corresponding tensor value
            *(r.data()+gidx_cm) = *(data()+gidx_rm);
        }

        // set the right flag to false
        r.right_ = true;

        return r;
    }

protected:      // member variables

    S* array_;  ///< data

    Size size_;  ///< flattened size

    std::vector<Size> shape_;  ///< tensor shape is a vector of sizes per dimension
    std::vector<Size> strides_;  ///< tensor strides precomputed at construction
    std::vector<Size> strides_rev_;  ///< tensor of reversed strides precomputed at construction

    bool right_; ///< right memory layout? (as in Fortran)
    bool own_;   ///< ownership    
};

//----------------------------------------------------------------------------------------------------------------------

typedef Tensor<double> TensorDouble;
typedef Tensor<float>  TensorFloat;

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

//----------------------------------------------------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
