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

/// @file   fam_exception.h
/// @author Metin Cakircali
/// @date   Mar 2026

/// @brief  Mock of OpenFAM's fam_exception.h

#pragma once

#include <exception>
#include <string>
#include <utility>

namespace openfam {

//----------------------------------------------------------------------------------------------------------------------

/// Mirror of OpenFAM error codes
enum Fam_Error {
    FAM_SUCCESS          = 0,
    FAM_ERR_UNKNOWN      = 1,
    FAM_ERR_NOTFOUND     = 2,
    FAM_ERR_ALREADYEXIST = 3,
    FAM_ERR_NOPERM       = 4,
    FAM_ERR_INVALID      = 5,
    FAM_ERR_NO_SPACE     = 6,
    FAM_ERR_OUTOFRANGE   = 7,
    FAM_ERR_METADATA     = 8,
    FAM_ERR_RPC          = 9,
    FAM_ERR_TIMEOUT      = 10,
    FAM_ERR_RESOURCE     = 11,
    FAM_ERR_LIBFABRIC    = 12,
    FAM_ERR_SHM          = 13,
    FAM_ERR_GRPC         = 14,
    FAM_ERR_PMI          = 15,
    FAM_ERR_UNIMPL       = 16,
};

//----------------------------------------------------------------------------------------------------------------------

// Mirror of OpenFAM's Fam_Exception. Adds fam_error() method to retrieve the error code.
class Fam_Exception : public std::exception {
public:

    explicit Fam_Exception(const char* msg, Fam_Error err = FAM_ERR_UNKNOWN) :
        message_(msg ? msg : "OpenFAM mock exception"), error_(err) {}

    explicit Fam_Exception(std::string msg, Fam_Error err = FAM_ERR_UNKNOWN) : message_(std::move(msg)), error_(err) {}

    const char* what() const noexcept override { return message_.c_str(); }
    const char* fam_error_msg() const noexcept { return message_.c_str(); }
    Fam_Error fam_error() const noexcept { return error_; }

private:

    std::string message_;
    Fam_Error error_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace openfam
