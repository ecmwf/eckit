// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_Status_h
#define eckit_mpi_Status_h

#include <iosfwd>

#include "eckit/memory/Counted.h"

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

class StatusContent : public Counted {
public:

    ~StatusContent() override;

    virtual int source() const = 0;
    virtual int tag() const    = 0;
    virtual int error() const  = 0;

    virtual void print(std::ostream&) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

class NullStatus : public StatusContent {
public:

    ~NullStatus() override {}

    int source() const override { return -1; };
    int tag() const override { return -1; };
    int error() const override { return 1; };

    void print(std::ostream&) const override;
};

//----------------------------------------------------------------------------------------------------------------------


/// Status by construction has always a valid content_
/// @invariant content_ is not null

class Status {

public:  // methods

    /// Null Status constructor
    Status();

    /// @pre content is not null
    Status(StatusContent* content);

    ~Status();

    Status(const Status&);

    Status& operator=(const Status&);

    int source() const { return content_->source(); }
    int tag() const { return content_->tag(); }
    int error() const { return content_->error(); }

    template <class T>
    T& as() {
        return dynamic_cast<T&>(*content_);
    }

    operator bool() const { return not dynamic_cast<const NullStatus*>(content_); }

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Status& o) {
        o.print(s);
        return s;
    }

private:  // members

    StatusContent* content_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
