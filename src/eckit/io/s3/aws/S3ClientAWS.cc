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

#include <aws/core/auth/AWSCredentials.h>
#include <aws/s3/S3Client.h>
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

namespace helper {

void printError(const Aws::S3::S3Error& error, const std::string& msg) {
    Log::error() << msg << ", error: " << error.GetExceptionName() << ", msg: " << error.GetMessage()
                 << " Remote IP: " << error.GetRemoteHostIpAddress() << std::endl;
}

}  // namespace helper

//----------------------------------------------------------------------------------------------------------------------

S3ClientAWS::S3ClientAWS():
    S3Client(S3Session::instance().getContext(S3Types::AWS)), client_(std::make_unique<Aws::S3::S3Client>()) { }

S3ClientAWS::~S3ClientAWS() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::configure(const S3Config& config) {
    LOG_DEBUG_LIB(LibEcKit) << "Configure S3 AWS client." << std::endl;

    Aws::Client::ClientConfiguration configuration;

    // we are not an ec2 instance
    configuration.disableIMDS = true;

    // setup region
    configuration.region = config.region;
    if (!config.region.empty()) { configuration.region = config.region; }

    // configuration.proxyScheme = Aws::Http::Scheme::HTTPS;
    // configuration.verifySSL = false;

    // setup endpoint
    if (!config.endpoint.empty()) { configuration.endpointOverride = "http://" + config.endpoint; }
    if (config.port > 0) { configuration.endpointOverride += ":" + std::to_string(config.port); }

    // setup credentials
    Aws::Auth::AWSCredentials credentials(config.keyID, config.secret);

    // endpoint provider
    auto provider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(config.tag.c_str());

    client_ = std::make_unique<Aws::S3::S3Client>(credentials, provider, configuration);
}

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

auto S3ClientAWS::createBucket(const std::string& bucketName) const -> bool {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucketName);

    auto outcome = client_->CreateBucket(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Created bucket: " << bucketName << std::endl;
    } else {
        helper::printError(outcome.GetError(), "Failed to create bucket: " + bucketName);
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteBucket(const std::string& bucketName) const -> bool {
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucketName);

    auto outcome = client_->DeleteBucket(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted bucket: " << bucketName << std::endl;
    } else {
        helper::printError(outcome.GetError(), "Failed to delete bucket: " + bucketName);
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    std::vector<std::string> result;

    auto outcome = client_->ListBuckets();
    if (outcome.IsSuccess()) {
        for (auto& bucket : outcome.GetResult().GetBuckets()) { result.emplace_back(bucket.GetName()); }
    } else {
        helper::printError(outcome.GetError(), "Failed to list buckets!");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

auto S3ClientAWS::putObject(const std::string& bucketName, const std::string& objectName) const -> bool {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucketName);
    request.SetKey(objectName);

    const std::shared_ptr<Aws::IOStream> inputData = Aws::MakeShared<Aws::StringStream>("");
    // *inputData << objectContent.c_str();
    request.SetBody(inputData);

    auto outcome = client_->PutObject(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object '" << objectName << "' to bucket '" << bucketName << "'.";
    } else {
        helper::printError(outcome.GetError(), "Failed to put object: " + objectName + " to bucket: " + bucketName);
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteObject(const std::string& bucketName, const std::string& objectKey) const -> bool {
    Aws::S3::Model::DeleteObjectRequest request;

    request.WithKey(objectKey).WithBucket(bucketName);

    auto outcome = client_->DeleteObject(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Successfully deleted the object." << std::endl;
    } else {
        helper::printError(outcome.GetError(), "Failed to delete object: " + objectKey + " in bucket: " + bucketName);
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::deleteObjects(const std::string& bucketName, const std::vector<std::string>& objectKeys) const -> bool {
    Aws::S3::Model::DeleteObjectsRequest request;

    Aws::S3::Model::Delete deleteObject;
    for (const auto& objectKey : objectKeys) {
        deleteObject.AddObjects(Aws::S3::Model::ObjectIdentifier().WithKey(objectKey));
    }

    request.SetDelete(deleteObject);
    request.SetBucket(bucketName);

    auto outcome = client_->DeleteObjects(request);
    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted " << objectKeys.size() << " objects from bucket " << bucketName << std::endl;
        for (auto i = 0; i < objectKeys.size(); ++i) {
            LOG_DEBUG_LIB(LibEcKit) << "Deleted object: " << objectKeys[i] << std::endl;
        }
    } else {
        helper::printError(outcome.GetError(), "Failed to delete objects in bucket: " + bucketName);
    }

    return outcome.IsSuccess();
}

auto S3ClientAWS::listObjects(const std::string& bucketName) const -> std::vector<std::string> {
    std::vector<std::string> result;

    Aws::S3::Model::ListObjectsRequest request;

    request.WithBucket(bucketName);

    auto outcome = client_->ListObjects(request);
    if (outcome.IsSuccess()) {
        Aws::Vector<Aws::S3::Model::Object> objects = outcome.GetResult().GetContents();
        for (Aws::S3::Model::Object& object : objects) { result.emplace_back(object.GetKey()); }
    } else {
        helper::printError(outcome.GetError(), "Failed to list objects in bucket: " + bucketName);
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
