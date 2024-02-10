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
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

#include "eckit/io/s3/aws/S3ClientAWS.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ContextAWS.h"

#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/utils/stream/PreallocatedStreamBuf.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/Delete.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/DeleteObjectsRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <iostream>
#include <memory>

//----------------------------------------------------------------------------------------------------------------------

namespace {

inline std::string awsErrorMessage(const std::string& msg, const Aws::S3::S3Error& error) {
    std::ostringstream oss;
    oss << msg << " AWS: " << error.GetMessage() << " Remote IP: " << error.GetRemoteHostIpAddress();
    return oss.str();
}

}  // namespace

namespace eckit {

const char* ALLOC_TAG = "S3ClientAWS";

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
    configuration.region = config.region();
    if (!config.region().empty()) { configuration.region = config.region(); }

    // configuration.proxyScheme = Aws::Http::Scheme::HTTPS;
    // configuration.verifySSL = false;

    // setup endpoint
    /// @todo handle http/https possibly via scheme flag in S3Config
    const auto& endpoint = config.endpoint();
    if (!endpoint.host().empty()) { configuration.endpointOverride = "http://" + endpoint.host(); }
    if (endpoint.port() > 0) { configuration.endpointOverride += ":" + std::to_string(endpoint.port()); }

    // setup credentials
    Aws::Auth::AWSCredentials credentials;
    if (auto cred = S3Session::instance().getCredentials(endpoint.host())) {
        credentials.SetAWSAccessKeyId(cred->keyID);
        credentials.SetAWSSecretKey(cred->secret);
    }

    // endpoint provider
    auto provider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(config.tag().c_str());

    // finally client
    client_ = std::make_unique<Aws::S3::S3Client>(credentials, provider, configuration);
}

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

void S3ClientAWS::createBucket(const std::string& bucketName) const {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucketName);

    auto outcome = client_->CreateBucket(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Created bucket=" << bucketName << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to create bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::deleteBucket(const std::string& bucketName) const {
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucketName);

    const auto outcome = client_->DeleteBucket(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted bucket=" << bucketName << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to delete bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

auto S3ClientAWS::bucketExists(const std::string& bucketName) const -> bool {
    Aws::S3::Model::HeadBucketRequest request;
    request.SetBucket(bucketName);
    return client_->HeadBucket(request).IsSuccess();
}

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    std::vector<std::string> result;

    auto outcome = client_->ListBuckets();

    if (outcome.IsSuccess()) {
        for (const auto& bucket : outcome.GetResult().GetBuckets()) { result.emplace_back(bucket.GetName()); }
    } else {
        Log::warning() << "Failed to list buckets!" << outcome.GetError();
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

void S3ClientAWS::putObject(const std::string& bucketName, const std::string& objectName, const void* buffer,
                            const uint64_t length) const {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucketName);
    request.SetKey(objectName);
    request.SetContentLength(length);

    auto sBuffer = Aws::New<Aws::Utils::Stream::PreallocatedStreamBuf>(ALLOC_TAG, (unsigned char*)buffer, length);
    auto sReader = Aws::MakeShared<Aws::IOStream>(ALLOC_TAG, sBuffer);
    request.SetBody(sReader);

    auto outcome = client_->PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object=" << objectName << " to bucket=" << bucketName << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to put object=" + objectName + " to bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::putObject(const std::string& bucketName, const std::string& objectName) const {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucketName);
    request.SetKey(objectName);

    // empty object
    request.SetBody(Aws::MakeShared<Aws::StringStream>(ALLOC_TAG));

    auto outcome = client_->PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object=" << objectName << " to bucket=" << bucketName << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to put object=" + objectName + " to bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::deleteObject(const std::string& bucketName, const std::string& objectKey) const {
    Aws::S3::Model::DeleteObjectRequest request;

    request.WithKey(objectKey).WithBucket(bucketName);

    auto outcome = client_->DeleteObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << objectKey << " in bucket=" << bucketName << std::endl;
    } else {
        auto msg =
            awsErrorMessage("Failed to delete object=" + objectKey + " in bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::deleteObjects(const std::string& bucketName, const std::vector<std::string>& objectKeys) const {
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
        for (const auto& object : objectKeys) { LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << object << std::endl; }
    } else {
        auto msg = awsErrorMessage("Failed to delete objects in bucket=" + bucketName, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

auto S3ClientAWS::listObjects(const std::string& bucketName) const -> std::vector<std::string> {
    std::vector<std::string> result;

    Aws::S3::Model::ListObjectsRequest request;

    request.WithBucket(bucketName);

    auto outcome = client_->ListObjects(request);

    if (outcome.IsSuccess()) {
        const auto& objects = outcome.GetResult().GetContents();
        for (const auto& object : objects) { result.emplace_back(object.GetKey()); }
    } else {
        Log::warning() << "Failed to list objects in bucket: " << bucketName << outcome.GetError();
    }

    return result;
}

auto S3ClientAWS::objectExists(const std::string& bucketName, const std::string& objectKey) const -> bool {
    Aws::S3::Model::HeadObjectRequest request;
    request.WithKey(objectKey).WithBucket(bucketName);
    return client_->HeadObject(request).IsSuccess();
}

auto S3ClientAWS::objectSize(const std::string& bucketName, const std::string& objectKey) const -> Length {
    Length result;

    Aws::S3::Model::HeadObjectRequest request;
    request.WithKey(objectKey).WithBucket(bucketName);

    const auto outcome = client_->HeadObject(request);

    if (outcome.IsSuccess()) {
        result = outcome.GetResult().GetContentLength();
    } else {
        const auto msg = awsErrorMessage("Object '" + objectKey + "' doesn't exist or no access!", outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
