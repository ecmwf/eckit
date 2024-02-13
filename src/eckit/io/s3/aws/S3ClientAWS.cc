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
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <iostream>
#include <memory>

namespace eckit {

const auto ALLOC_TAG = "S3ClientAWS";

//----------------------------------------------------------------------------------------------------------------------

namespace {

inline std::string awsErrorMessage(const std::string& msg, const Aws::S3::S3Error& error) {
    std::ostringstream oss;
    oss << msg << " AWS: " << error.GetMessage() << " Remote IP: " << error.GetRemoteHostIpAddress();
    return oss.str();
}

class BufferIOStream: public Aws::IOStream {
public:
    /// @todo reinterpret_cast
    BufferIOStream(void* buffer, const uint64_t size):
        Aws::IOStream(new Aws::Utils::Stream::PreallocatedStreamBuf(reinterpret_cast<unsigned char*>(buffer), size)) { }

    ~BufferIOStream() override { delete rdbuf(); }
};

}  // namespace

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
    // configuration.region = config.region;
    if (!config.region.empty()) { configuration.region = config.region; }

    // configuration.proxyScheme = Aws::Http::Scheme::HTTPS;
    // configuration.verifySSL = false;

    // setup endpoint
    /// @todo handle http/https possibly via scheme flag in S3Config
    // const auto& endpoint = config.endpoint;
    if (!config.endpoint.host().empty()) { configuration.endpointOverride = "http://" + config.endpoint.host(); }
    if (config.endpoint.port() > 0) { configuration.endpointOverride += ":" + std::to_string(config.endpoint.port()); }

    // setup credentials
    Aws::Auth::AWSCredentials credentials;
    if (auto cred = S3Session::instance().getCredentials(config.endpoint.host())) {
        credentials.SetAWSAccessKeyId(cred->keyID);
        credentials.SetAWSSecretKey(cred->secret);
    }

    // endpoint provider
    auto provider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ALLOC_TAG);

    // finally client
    client_ = std::make_unique<Aws::S3::S3Client>(credentials, provider, configuration);
}

void S3ClientAWS::print(std::ostream& out) const {
    S3Client::print(out);
    out << "S3ClientAWS[]";
}

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

void S3ClientAWS::createBucket(const std::string& bucket) const {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = client_->CreateBucket(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Created bucket=" << bucket << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to create bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::emptyBucket(const std::string& bucket) const {
    deleteObjects(bucket, listObjects(bucket));
}

void S3ClientAWS::deleteBucket(const std::string& bucket) const {
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = client_->DeleteBucket(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted bucket=" << bucket << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to delete bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

auto S3ClientAWS::bucketExists(const std::string& bucket) const -> bool {
    Aws::S3::Model::HeadBucketRequest request;
    request.SetBucket(bucket);
    return client_->HeadBucket(request).IsSuccess();
}

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    std::vector<std::string> buckets;

    auto outcome = client_->ListBuckets();

    if (outcome.IsSuccess()) {
        for (const auto& bucket : outcome.GetResult().GetBuckets()) { buckets.emplace_back(bucket.GetName()); }
    } else {
        Log::warning() << "Failed to list buckets!" << outcome.GetError();
    }

    return buckets;
}

//----------------------------------------------------------------------------------------------------------------------
// PUT OBJECT

void S3ClientAWS::putObject(const std::string& bucket, const std::string& object) const {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);

    // empty object
    request.SetBody(Aws::MakeShared<Aws::StringStream>(ALLOC_TAG));

    auto outcome = client_->PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object=" << object << " to bucket=" << bucket << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to put object=" + object + " to bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

auto S3ClientAWS::putObject(const std::string& bucket, const std::string& object, const void* buffer,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);
    // request.SetContentLength(length);

    auto streamBuffer = Aws::MakeShared<BufferIOStream>(ALLOC_TAG, const_cast<void*>(buffer), length);
    request.SetBody(streamBuffer);

    auto outcome = client_->PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Added object=" << object << " to bucket=" << bucket << std::endl;
        /// @todo actual size of written bytes
        return length;
        // return request.GetContentLength();
    } else {
        auto msg = awsErrorMessage("Failed to put object=" + object + " to bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

// auto S3ClientAWS::putObject(const std::string& bucket, const std::string& object, const void* buffer,
//                             const uint64_t start, const uint64_t end) const -> long long {
//     Aws::S3::Model::PutObjectRequest request;
//
//     request.SetBucket(bucket);
//     request.SetKey(object);
//     // request.SetContentLength(length);
//
//     auto streamBuffer = Aws::MakeShared<BufferIOStream>(ALLOC_TAG, const_cast<void*>(buffer), length);
//     request.SetBody(streamBuffer);
//
//     auto outcome = client_->PutObject(request);
//
//     if (outcome.IsSuccess()) {
//         LOG_DEBUG_LIB(LibEcKit) << "Added object=" << object << " to bucket=" << bucket << std::endl;
//         /// @todo actual size of written bytes
//         return end - start;
//         // return request.GetContentLength();
//     }
//
//     auto msg = awsErrorMessage("Failed to put object=" + object + " to bucket=" + bucket, outcome.GetError());
//     throw S3SeriousBug(msg, Here());
// }

//----------------------------------------------------------------------------------------------------------------------
// GET OBJECT

auto S3ClientAWS::getObject(const std::string& bucket, const std::string& object, void* buffer,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::GetObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);
    request.SetResponseStreamFactory([&buffer, length]() { return Aws::New<BufferIOStream>(ALLOC_TAG, buffer, length); });

    auto outcome = client_->GetObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Retrieved object=" << object << " from bucket=" << bucket << std::endl;
        return outcome.GetResult().GetContentLength();
    }

    auto msg = awsErrorMessage("Failed to retrieve object=" + object + " to bucket=" + bucket, outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

auto S3ClientAWS::getObject(const std::string& bucket, const std::string& object, void* buffer, const uint64_t offset,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::GetObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);
    request.SetResponseStreamFactory([&buffer, length]() { return Aws::New<BufferIOStream>(ALLOC_TAG, buffer, length); });
    request.SetRange(std::to_string(offset) + "-" + std::to_string(offset + length));

    auto outcome = client_->GetObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Retrieved object=" << object << " from bucket=" << bucket << std::endl;
        return outcome.GetResult().GetContentLength();
    }

    auto msg = awsErrorMessage("Failed to retrieve object=" + object + " to bucket=" + bucket, outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// OBJECT

void S3ClientAWS::deleteObject(const std::string& bucket, const std::string& object) const {
    Aws::S3::Model::DeleteObjectRequest request;

    request.WithKey(object).WithBucket(bucket);

    auto outcome = client_->DeleteObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << object << " in bucket=" << bucket << std::endl;
    } else {
        auto msg = awsErrorMessage("Failed to delete object=" + object + " in bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

void S3ClientAWS::deleteObjects(const std::string& bucket, const std::vector<std::string>& objects) const {
    if (objects.empty()) {
        LOG_DEBUG_LIB(LibEcKit) << "No objects to delete in bucket=" << bucket << std::endl;
        return;
    }

    Aws::S3::Model::DeleteObjectsRequest request;

    request.SetBucket(bucket);

    Aws::S3::Model::Delete deleteObject;
    for (const auto& object : objects) { deleteObject.AddObjects(Aws::S3::Model::ObjectIdentifier().WithKey(object)); }
    // deleteObject.SetQuiet(true);

    request.SetDelete(deleteObject);

    auto outcome = client_->DeleteObjects(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Deleted " << objects.size() << " objects in bucket=" << bucket << std::endl;
        for (const auto& object : objects) { LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << object << std::endl; }
    } else {
        auto msg = awsErrorMessage("Failed to delete objects in bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }
}

auto S3ClientAWS::listObjects(const std::string& bucket) const -> std::vector<std::string> {
    std::vector<std::string> result;

    Aws::S3::Model::ListObjectsRequest request;

    request.WithBucket(bucket);

    auto outcome = client_->ListObjects(request);

    if (outcome.IsSuccess()) {
        const auto& objects = outcome.GetResult().GetContents();
        for (const auto& object : objects) { result.emplace_back(object.GetKey()); }
    } else {
        Log::warning() << "Failed to list objects in bucket=" << bucket << outcome.GetError();
    }

    return result;
}

auto S3ClientAWS::objectExists(const std::string& bucket, const std::string& object) const -> bool {
    Aws::S3::Model::HeadObjectRequest request;
    request.WithKey(object).WithBucket(bucket);
    return client_->HeadObject(request).IsSuccess();
}

auto S3ClientAWS::objectSize(const std::string& bucket, const std::string& object) const -> Length {
    Length result;

    Aws::S3::Model::HeadObjectRequest request;
    request.WithKey(object).WithBucket(bucket);

    auto outcome = client_->HeadObject(request);

    if (outcome.IsSuccess()) {
        result = outcome.GetResult().GetContentLength();
    } else {
        const auto msg = awsErrorMessage("Object '" + object + "' doesn't exist or no access!", outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
