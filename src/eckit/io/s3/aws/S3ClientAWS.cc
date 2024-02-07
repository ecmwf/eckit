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
#include <aws/s3/model/Delete.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/DeleteObjectsRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <iostream>
#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ClientAWS::S3ClientAWS(const S3Config& config): S3Client(S3Session::instance().getContext(config.type)) {
    LOG_DEBUG_LIB(LibEcKit) << "S3ClientAWS::setConfiguration" << std::endl;

    // we are not an ec2 instance
    config_.disableIMDS = true;

    // setup region
    config_.region = config.region;
    if (!config.region.empty()) { config_.region = config.region; }

    // config_.proxyScheme = Aws::Http::Scheme::HTTPS;
    // config_.verifySSL = false;

    // setup endpoint
    if (!config.endpoint.empty()) { config_.endpointOverride = "http://" + config.endpoint; }
    if (config.port > 0) { config_.endpointOverride += ":" + std::to_string(config.port); }
}

S3ClientAWS::~S3ClientAWS() = default;

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

auto S3ClientAWS::createBucket(const std::string& bucketName) const -> bool {
    Aws::S3::S3Client client(config_);

    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucketName);

    // Change the bucket location constraint enum to target Region.
    if (config_.region != "eu-central-1") {
        Aws::S3::Model::CreateBucketConfiguration bucketConfig;
        bucketConfig.SetLocationConstraint(
            Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(config_.region));
        request.SetCreateBucketConfiguration(bucketConfig);
    }

    Aws::S3::Model::CreateBucketOutcome outcome = client.CreateBucket(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Created bucket: " << bucketName << std::endl;
    } else {
        const auto err = outcome.GetError();
        Log::error() << "Failed to create bucket: " << bucketName << " Error: " << err.GetExceptionName() << ": "
                     << err.GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteBucket(const std::string& bucketName) const -> bool {
    Aws::S3::S3Client client(config_);

    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucketName);

    Aws::S3::Model::DeleteBucketOutcome outcome = client.DeleteBucket(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted bucket: " << bucketName << std::endl;
    } else {
        const Aws::S3::S3Error& err = outcome.GetError();
        ///>>> Aaargh, you discard the errors. We need to throw an Exception if something goes wrong!!!
        Log::error() << "Failed to delete bucket: " << bucketName << " Error: " << err.GetExceptionName() << ": "
                     << err.GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    Aws::S3::S3Client client(config_);

    std::vector<std::string> result;

    auto outcome = client.ListBuckets();
    if (outcome.IsSuccess()) {
        for (auto& bucket : outcome.GetResult().GetBuckets()) { result.emplace_back(bucket.GetName()); }
    } else {
        ///>>> Aaargh, you discard the errors. We need to throw an Exception if something goes wrong!!!
        Log::error() << "Cannot list buckets! " << outcome.GetError().GetMessage() << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

auto S3ClientAWS::putObject(const std::string& bucketName, const std::string& objectName) const -> bool {
    Aws::S3::S3Client s3_client(config_);

    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(bucketName);
    // We are using the name of the file as the key for the object in the bucket.
    // However, this is just a string and can be set according to your retrieval needs.
    request.SetKey(objectName);

    const std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::StringStream>("");

    // *inputData << objectContent.c_str();

    request.SetBody(inputData);

    Aws::S3::Model::PutObjectOutcome outcome = s3_client.PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object '" << objectName << "' to bucket '" << bucketName << "'.";
    } else {
        Log::error() << "Error: putObject: " << outcome.GetError().GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteObject(const std::string& bucketName, const std::string& objectKey) const -> bool {
    Aws::S3::S3Client client(config_);

    Aws::S3::Model::DeleteObjectRequest request;
    request.WithKey(objectKey).WithBucket(bucketName);

    Aws::S3::Model::DeleteObjectOutcome outcome = client.DeleteObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Successfully deleted the object." << std::endl;
    } else {
        auto err = outcome.GetError();
        Log::error() << "Error: DeleteObject: " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteObjects(const std::string& bucketName, const std::vector<std::string>& objectKeys) const -> bool {
    Aws::S3::S3Client client(config_);

    Aws::S3::Model::DeleteObjectsRequest request;

    Aws::S3::Model::Delete deleteObject;
    for (const auto& objectKey : objectKeys) {
        deleteObject.AddObjects(Aws::S3::Model::ObjectIdentifier().WithKey(objectKey));
    }

    request.SetDelete(deleteObject);
    request.SetBucket(bucketName);

    Aws::S3::Model::DeleteObjectsOutcome outcome = client.DeleteObjects(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted " << objectKeys.size() << " objects from bucket " << bucketName << std::endl;
        for (auto i = 0; i < objectKeys.size(); ++i) {
            LOG_DEBUG_LIB(LibEcKit) << "Deleted object: " << objectKeys[i] << std::endl;
        }
    } else {
        const auto err = outcome.GetError();
        Log::error() << "Error deleting objects. " << err.GetExceptionName() << ": " << err.GetMessage() << std::endl;
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::listObjects(const std::string& bucketName) const -> std::vector<std::string> {
    Aws::S3::S3Client s3_client(config_);

    std::vector<std::string> result;

    Aws::S3::Model::ListObjectsRequest request;
    request.WithBucket(bucketName);

    auto outcome = s3_client.ListObjects(request);

    if (outcome.IsSuccess()) {
        Aws::Vector<Aws::S3::Model::Object> objects = outcome.GetResult().GetContents();
        for (Aws::S3::Model::Object& object : objects) { result.emplace_back(object.GetKey()); }
    } else {
        Log::error() << "Cannot list objects! " << outcome.GetError().GetMessage() << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
