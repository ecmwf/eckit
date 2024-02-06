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

#include "eckit/io/s3/aws/S3ClientAWS.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ContextAWS.h"

#include <aws/s3/model/CreateBucketRequest.h>

#include <iostream>
#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ClientAWS::S3ClientAWS(const S3Config& config): S3Client(S3Session::instance().getContext(config.type)) {
    LOG_DEBUG_LIB(LibEcKit) << "S3ClientAWS::setConfiguration" << std::endl;

    config_.region = config.region;

    // S3Config holds handles credentials

    // config_.proxyHost   = "127.0.0.1";
    // config_.proxyPort   = 9000;
    // config_.proxyScheme = Aws::Http::Scheme::HTTPS;
    // config_.verifySSL = false;
    config_.endpointOverride = "http://127.0.0.1:9000";
}

S3ClientAWS::~S3ClientAWS() = default;

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::createBucket(const std::string& name) const -> bool {
    Aws::S3::S3Client client(config_);

    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(name);

    // Change the bucket location constraint enum to target Region.
    if (config_.region != "eu-central-1") {
        Aws::S3::Model::CreateBucketConfiguration bucketConfig;
        bucketConfig.SetLocationConstraint(
            Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(config_.region));
        request.SetCreateBucketConfiguration(bucketConfig);
    }

    Aws::S3::Model::CreateBucketOutcome outcome = client.CreateBucket(request);
    if (!outcome.IsSuccess()) {
        auto err = outcome.GetError();
        Log::error() << "Failed to create bucket [" << name << "]! " << err.GetExceptionName() << ": "
                     << err.GetMessage() << std::endl;
    } else {
        LOG_DEBUG_LIB(LibEcKit) << "Created bucket " << name << std::endl;
    }

    return outcome.IsSuccess();
}

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::listBuckets() const {
    Aws::S3::S3Client client(config_);

    auto outcome = client.ListBuckets();
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Found " << outcome.GetResult().GetBuckets().size() << " buckets:\n";
        for (auto& bucket : outcome.GetResult().GetBuckets()) {
            LOG_DEBUG_LIB(LibEcKit) << bucket.GetName() << std::endl;
        }
    } else {
        Log::error() << "List failed! Error: " << outcome.GetError();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
