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

public:  // methods
    
    /// Default constructor (empty tensor)
    Tensor() : array_(0), size_(0), shape_(0), strides_(0), own_(false) {}

    /// Construct tensor with given rows and columns (allocates memory, not initialised)
    Tensor(const std::vector<Size>& shape) : array_(nullptr), shape_(shape), strides_(strides(shape)), own_(true) {
        size_ = flatten(shape_);
        ASSERT(size() > 0);
        array_ = new S[size_];
        ASSERT(array_);
    }

    /// Construct tensor from existing data (does NOT take ownership)
    Tensor(const S* array, const std::vector<Size>& shape) :
        array_(const_cast<S*>(array)), strides_(strides(shape)), own_(false) {
        shape_ = shape;
        size_  = flatten(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
    }

    /// Constructor from Stream
    Tensor(Stream& s) : array_(0), size_(0), shape_(0), own_(true) {
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
    }

    /// Copy constructor
    Tensor(const Tensor& other) : array_(new S[other.size()]), size_(other.size_), shape_(other.shape_), strides_(other.strides_), own_(true) {
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

    void move(Tensor&& other) { swap(other); }

    Tensor transformRigthToLeftLayout() const {
        Tensor r(shape_);

        // implement here ....
        NOTIMP;

        return r;
    }

    Tensor transformLeftToRightLayout() const {
        Tensor r(shape_);

        // implement here ....
        NOTIMP;

        return r;
    }

protected:      // member variables
    S* array_;  ///< data

    Size size_;  ///< flattened size

    std::vector<Size> shape_;  ///< tensor shape is a vector of sizes per dimension
    std::vector<Size> strides_;  ///< tensor strides precomputed at construction

    bool own_;   ///< ownership
    bool right_; ///< right memory layout? (as in Fortran)
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
