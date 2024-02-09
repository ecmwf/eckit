/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "eckit/io/s3/S3Name.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/URI.h"
#include "eckit/filesystem/URIManager.h"
#include "eckit/io/s3/S3Client.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Handle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3Name::S3Name(const URI& uri): config_(uri) {
    LOG_DEBUG_LIB(LibEcKit) << "URI -----> " << uri << std::endl;
    LOG_DEBUG_LIB(LibEcKit) << "URI -----> host=" << uri.host() << std::endl;
    LOG_DEBUG_LIB(LibEcKit) << "    -----> " << *this << std::endl;
}

S3Name::S3Name(const S3Config& config, const std::string& bucketName, const std::string& objectName):
    config_(config), bucket_(bucketName), object_(objectName) { }

S3Name::~S3Name() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3Name::print(std::ostream& out) const {
    out << "S3Name[config=" << config_ << ",bucket=" << bucket_ << ",object=" << object_ << "]";
}

auto S3Name::exists() -> bool {
    NOTIMP;
    return false;
}

auto S3Name::dataHandle() -> std::unique_ptr<DataHandle> {
    LOG_DEBUG_LIB(LibEcKit) << "dataHandle " << *this << std::endl;
    return std::make_unique<S3Handle>(*this);
}

//----------------------------------------------------------------------------------------------------------------------
// S3 URI MANAGER

class S3URIManager: public URIManager {
    bool query() override { return false; }
    bool fragment() override { return false; }

    bool exists(const URI& uri) override { return S3Name(uri).exists(); }

    DataHandle* newWriteHandle(const URI& uri) override { return S3Name(uri).dataHandle().release(); }

    DataHandle* newReadHandle(const URI& uri) override { return S3Name(uri).dataHandle().release(); }

    DataHandle* newReadHandle(const URI& uri, const OffsetList& ol, const LengthList& ll) override {
        NOTIMP;
        return S3Name(uri).dataHandle().release();
    }

    std::string asString(const URI& uri) const override {
        std::string q = uri.query();
        if (!q.empty()) { q = "?" + q; }
        std::string f = uri.fragment();
        if (!f.empty()) { f = "#" + f; }

        return uri.name() + q + f;
    }

public:
    S3URIManager(const std::string& name): URIManager(name) { }
};

//----------------------------------------------------------------------------------------------------------------------

static S3URIManager manager_s3("s3");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
