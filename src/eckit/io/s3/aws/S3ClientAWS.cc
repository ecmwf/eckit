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

#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/utils/stream/PreallocatedStreamBuf.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/Delete.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/DeleteObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
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

class BufferIOStream: public Aws::IOStream {
public:
    BufferIOStream(void* buffer, const uint64_t length):
        Aws::IOStream(new Aws::Utils::Stream::PreallocatedStreamBuf(reinterpret_cast<unsigned char*>(buffer), length)) {
    }

    ~BufferIOStream() override { delete rdbuf(); }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

const auto ALLOC_TAG = "S3ClientAWS";

S3ClientAWS::S3ClientAWS(const S3Config& config): S3Client(config, S3Session::instance().getContext(S3Types::AWS)) {
    configure(config);
}

S3ClientAWS::~S3ClientAWS() = default;

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::configure(const S3Config& config) {
    LOG_DEBUG_LIB(LibEcKit) << "Configure S3 AWS client..." << std::endl;

    Aws::Client::ClientConfigurationInitValues initVal;
    initVal.shouldDisableIMDS = true;
    Aws::Client::ClientConfiguration configuration(initVal);

    // we are not an ec2 instance
    configuration.disableIMDS   = true;
    configuration.disableImdsV1 = true;

    // setup region
    // configuration.region = config.region;
    if (!config.region.empty()) { configuration.region = config.region; }

    // configuration.scheme = Aws::Http::Scheme::HTTPS;
    // configuration.verifySSL = false;

    // setup endpoint
    /// @todo handle http/https possibly via scheme flag in S3Config
    if (!config.endpoint.host().empty()) { configuration.endpointOverride = "http://" + config.endpoint.host(); }
    if (config.endpoint.port() > 0) { configuration.endpointOverride += ":" + std::to_string(config.endpoint.port()); }

    if (auto cred = S3Session::instance().getCredentials(config.endpoint.host())) {
        // credentials provider
        auto cProvider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(ALLOC_TAG, cred->keyID, cred->secret);
        // endpoint provider
        auto eProvider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(ALLOC_TAG);
        // client
        client_ = std::make_unique<Aws::S3::S3Client>(cProvider, eProvider, configuration);
    } else {
        throw S3SeriousBug("No credentials found!", Here());
    }
}

void S3ClientAWS::print(std::ostream& out) const {
    S3Client::print(out);
    out << "S3ClientAWS[]";
}

auto S3ClientAWS::getClient() const -> Aws::S3::S3Client& {
    if (client_) { return *client_; }
    throw S3SeriousBug("Invalid client!", Here());
}

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

void S3ClientAWS::createBucket(const std::string& bucket) const {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = getClient().CreateBucket(request);

    if (!outcome.IsSuccess()) {
        const auto& err   = outcome.GetError();
        const auto  msg   = awsErrorMessage("Failed to create bucket=" + bucket, err);
        const auto  eType = err.GetErrorType();
        if (eType == Aws::S3::S3Errors::BUCKET_ALREADY_EXISTS || eType == Aws::S3::S3Errors::BUCKET_ALREADY_OWNED_BY_YOU) {
            throw S3EntityAlreadyExists(msg, Here());
        }
        throw S3SeriousBug(msg, Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "Created bucket=" << bucket << std::endl;
}

void S3ClientAWS::emptyBucket(const std::string& bucket) const {
    deleteObjects(bucket, listObjects(bucket));
}

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::deleteBucket(const std::string& bucket) const {
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = getClient().DeleteBucket(request);

    if (!outcome.IsSuccess()) {
        const auto& err = outcome.GetError();
        const auto  msg = awsErrorMessage("Failed to delete bucket=" + bucket, err);
        if (err.GetErrorType() == Aws::S3::S3Errors::NO_SUCH_BUCKET) { throw S3EntityNotFound(msg, Here()); }
        if (err.GetErrorType() == Aws::S3::S3Errors::UNKNOWN && err.GetExceptionName() == "BucketNotEmpty") {
            throw S3BucketNotEmpty(msg, Here());
        }
        throw S3SeriousBug(msg, Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "Deleted bucket=" << bucket << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::bucketExists(const std::string& bucket) const -> bool {
    Aws::S3::Model::HeadBucketRequest request;

    request.SetBucket(bucket);

    return getClient().HeadBucket(request).IsSuccess();
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    auto outcome = getClient().ListBuckets();

    if (outcome.IsSuccess()) {
        std::vector<std::string> buckets;
        for (const auto& bucket : outcome.GetResult().GetBuckets()) { buckets.emplace_back(bucket.GetName()); }
        return buckets;
    }

    auto msg = awsErrorMessage("Failed list buckets!", outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// PUT OBJECT

auto S3ClientAWS::putObject(const std::string& bucket, const std::string& object, const void* buffer,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);
    // request.SetContentLength(length);

    if (buffer && length > 0) {
        auto streamBuffer = Aws::MakeShared<BufferIOStream>(ALLOC_TAG, const_cast<void*>(buffer), length);
        request.SetBody(streamBuffer);
    } else {
        // empty object
        request.SetBody(Aws::MakeShared<Aws::StringStream>(ALLOC_TAG));
    }

    auto outcome = getClient().PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Put object=" << object << " [len=" << length << "] to bucket=" << bucket << std::endl;
        /// @todo actual size of written bytes
        return length;
    }

    auto msg = awsErrorMessage("Failed to put object=" + object + " to bucket=" + bucket, outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// GET OBJECT

auto S3ClientAWS::getObject(const std::string& bucket, const std::string& object, void* buffer, const uint64_t offset,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::GetObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);
    request.SetResponseStreamFactory([&buffer, length]() { return Aws::New<BufferIOStream>(ALLOC_TAG, buffer, length); });
    /// @todo range and streambuf
    // request.SetRange(std::to_string(offset) + "-" + std::to_string(offset + length));

    auto outcome = getClient().GetObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Get object=" << object << " from bucket=" << bucket << std::endl;
        LOG_DEBUG_LIB(LibEcKit) << "Req. len=" << length << ", Obj. len=" << outcome.GetResult().GetContentLength()
                                << std::endl;
        return outcome.GetResult().GetContentLength();
    }

    auto msg = awsErrorMessage("Failed to retrieve object=" + object + " from bucket=" + bucket, outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT

void S3ClientAWS::deleteObject(const std::string& bucket, const std::string& object) const {
    Aws::S3::Model::DeleteObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);

    auto outcome = getClient().DeleteObject(request);

    if (!outcome.IsSuccess()) {
        auto msg = awsErrorMessage("Failed to delete object=" + object + " in bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << object << " in bucket=" << bucket << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::deleteObjects(const std::string& bucket, const std::vector<std::string>& objects) const {
    if (objects.empty()) { return; }

    Aws::S3::Model::DeleteObjectsRequest request;

    request.SetBucket(bucket);

    Aws::S3::Model::Delete deleteObject;
    for (const auto& object : objects) { deleteObject.AddObjects(Aws::S3::Model::ObjectIdentifier().WithKey(object)); }
    // deleteObject.SetQuiet(true);

    request.SetDelete(deleteObject);

    auto outcome = getClient().DeleteObjects(request);

    if (!outcome.IsSuccess()) {
        auto msg = awsErrorMessage("Failed to delete objects in bucket=" + bucket, outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "Deleted " << objects.size() << " objects in bucket=" << bucket << std::endl;
    for (const auto& object : objects) { LOG_DEBUG_LIB(LibEcKit) << "Deleted object=" << object << std::endl; }
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::listObjects(const std::string& bucket) const -> std::vector<std::string> {
    Aws::S3::Model::ListObjectsV2Request request;

    request.SetBucket(bucket);

    auto outcome = getClient().ListObjectsV2(request);

    if (!outcome.IsSuccess()) {
        const auto& err = outcome.GetError();
        const auto  msg = awsErrorMessage("Failed to list objects in bucket=" + bucket, err);
        if (err.GetErrorType() == Aws::S3::S3Errors::NO_SUCH_BUCKET) { throw S3EntityNotFound(msg, Here()); }
        throw S3SeriousBug(msg, Here());
    }

    std::vector<std::string> result;

    const auto& objects = outcome.GetResult().GetContents();
    for (const auto& object : objects) { result.emplace_back(object.GetKey()); }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::objectExists(const std::string& bucket, const std::string& object) const -> bool {
    Aws::S3::Model::HeadObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);

    return getClient().HeadObject(request).IsSuccess();
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::objectSize(const std::string& bucket, const std::string& object) const -> long long {
    Aws::S3::Model::HeadObjectRequest request;

    request.SetBucket(bucket);
    request.SetKey(object);

    auto outcome = getClient().HeadObject(request);

    if (outcome.IsSuccess()) { return outcome.GetResult().GetContentLength(); }

    const auto msg = awsErrorMessage("Object '" + object + "' doesn't exist or no access!", outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
