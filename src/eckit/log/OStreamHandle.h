/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file OStreamHandle.h
/// @author Tiago Quintino

#ifndef eckit_log_OStreamHandle_h
#define eckit_log_OStreamHandle_h

#include <iosfwd>
#include <cassert>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ostream_handle {
public:
    
    explicit ostream_handle() : ptr_(0), owned_(false) {}
    
    explicit ostream_handle( std::ostream* os ) : ptr_(os), owned_(true) {}
    
    explicit ostream_handle( std::ostream& os ) : ptr_(&os), owned_(false) {}

    ~ostream_handle(){ release(); }
    
    /// copy transfers ownership
    ostream_handle(const ostream_handle& o) 
    { 
        ptr_ = o.ptr_;
        owned_ = o.owned_;
        o.owned_ = false;
    }

    /// assigment transfers ownership
    ostream_handle& operator=(const ostream_handle& o)
    {
        release();
        ptr_ = o.ptr_;
        owned_ = o.owned_;
        o.owned_ = false;
        return *this;
    }

    std::ostream* get() const { return ptr_; }
    
    void reset( std::ostream* os )
    {
        release();
        ptr_ = os;
        owned_ = true;
    }

    void reset( std::ostream& os)
    {
        release();
        ptr_ = &os;
        owned_ = false;
    }

    /// Dereferences the pointee
    std::ostream& operator*() const { assert(ptr_); return *ptr_; }

    /// Calling operator
    std::ostream* operator->() const { assert(ptr_); return ptr_; }

    /// @returns true if pointer is not null
    operator bool() const { return valid(); }

private:
    
    bool valid() const { return (ptr_ != 0); }
    void release() 
    {
        if( owned_ && valid() ) delete ptr_; 
        ptr_ = 0; 
    }
    
    std::ostream* ptr_;
    mutable bool owned_;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
