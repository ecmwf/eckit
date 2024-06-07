/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamObjectName.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/fam/FamName.h"

namespace eckit {

class DataHandle;
class FamObjectDetail;

//----------------------------------------------------------------------------------------------------------------------

class FamObjectName: public FamName {
public:  // methods
    explicit FamObjectName(const URI& uri);

    FamObjectName(const net::Endpoint& endpoint, std::string regionName, std::string objectName);

    auto size() const -> fam::size_t;

    auto exists() -> bool override;

    auto lookup() -> bool override;

    void create(const FamProperty& property) override;

    void destroy() override;

    void put(const void* buffer, fam::size_t offset, fam::size_t length) const;

    void get(void* buffer, fam::size_t offset, fam::size_t length) const;

    [[nodiscard]]
    auto dataHandle(const Offset& offset = 0) const -> DataHandle*;

    [[nodiscard]]
    auto dataHandle(const Offset& offset, const Length& length) const -> DataHandle*;

    auto objectName() const -> const std::string& { return objectName_; }

    auto regionName() const -> const std::string& { return regionName_; }

    auto asString() const -> std::string override;

private:  // methods
    void print(std::ostream& out) const override;

private:  // members
    std::string regionName_;
    std::string objectName_;

    std::shared_ptr<FamObjectDetail> object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
