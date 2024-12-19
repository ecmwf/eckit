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
#include "eckit/exception/Exceptions.h"
#include "eckit/io/s3/S3Credential.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/S3ObjectPath.h"
#include "eckit/io/s3/S3Session.h"
#include "eckit/io/s3/aws/S3ContextAWS.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"
#include "eckit/net/IPAddress.h"

#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSAllocator.h>
#include <aws/core/utils/memory/stl/AWSStreamFwd.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/stream/PreallocatedStreamBuf.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3EndpointProvider.h>
#include <aws/s3/S3Errors.h>
#include <aws/s3/model/CreateBucketConfiguration.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/Delete.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/DeleteObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/ObjectIdentifier.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

namespace {

inline std::string awsErrorMessage(const std::string& msg, const Aws::S3::S3Error& error) {
    std::ostringstream oss;
    oss << msg << " AWS: " << error.GetMessage() << " Remote IP: " << error.GetRemoteHostIpAddress();
    return oss.str();
}

class BufferIOStream : public Aws::IOStream {
public:
    BufferIOStream(void* buffer, const uint64_t length)
        : Aws::IOStream(new Aws::Utils::Stream::PreallocatedStreamBuf(reinterpret_cast<unsigned char*>(buffer), length)) {
    }

    ~BufferIOStream() override { delete rdbuf(); }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

const auto allocTag = "S3ClientAWS";

S3ClientAWS::S3ClientAWS(const S3Config& config) : S3Client(config), ctx_ {S3ContextAWS::instance()} { }

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::configure() const {

    LOG_DEBUG_LIB(LibEcKit) << "Configure S3 AWS client... ";

    Aws::Client::ClientConfigurationInitValues initVal;
    initVal.shouldDisableIMDS = true;
    Aws::Client::ClientConfiguration configAWS(initVal);

    // we are not an ec2 instance
    configAWS.disableIMDS   = true;
    configAWS.disableImdsV1 = true;

    // setup region
    if (!config().region.empty()) { configAWS.region = config().region; }

    // configuration.scheme    = Aws::Http::Scheme::HTTPS;
    configAWS.verifySSL = false;

    // setup endpoint
    if (config().endpoint.host().empty()) { throw UserError("Empty endpoint hostname in configuration!", Here()); }

    configAWS.endpointOverride = net::IPAddress::hostAddress(config().endpoint.host()).asString();

    ASSERT(config().endpoint.port() > 0);
    configAWS.endpointOverride += ":" + std::to_string(config().endpoint.port());

    LOG_DEBUG_LIB(LibEcKit) << "endpoint=" << configAWS.endpointOverride << std::endl;

    const auto cred = S3Session::instance().getCredential(config().endpoint);
    // credentials provider
    auto cProvider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(allocTag, cred->keyID, cred->secret);
    // endpoint provider
    auto eProvider = Aws::MakeShared<Aws::S3::Endpoint::S3EndpointProvider>(allocTag);
    // client
    client_ = std::make_unique<Aws::S3::S3Client>(cProvider, eProvider, configAWS);
}

auto S3ClientAWS::client() const -> Aws::S3::S3Client& {
    if (!client_) { configure(); }
    if (!client_) { throw S3SeriousBug("Invalid client!", Here()); }
    return *client_;
}

//----------------------------------------------------------------------------------------------------------------------
// BUCKET

void S3ClientAWS::createBucket(const std::string& bucket) const {
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = client().CreateBucket(request);

    if (!outcome.IsSuccess()) {
        const auto& err   = outcome.GetError();
        const auto  msg   = awsErrorMessage("Failed to create bucket=" + bucket, err);
        const auto  eType = err.GetErrorType();
        if (eType == Aws::S3::S3Errors::BUCKET_ALREADY_EXISTS || eType == Aws::S3::S3Errors::BUCKET_ALREADY_OWNED_BY_YOU) {
            throw S3EntityAlreadyExists(msg, Here());
        }
        throw S3SeriousBug(msg, Here());
    }

    /// @todo do we wait for the bucket to propagate?

    LOG_DEBUG_LIB(LibEcKit) << "Created bucket=" << bucket << std::endl;
}

void S3ClientAWS::emptyBucket(const std::string& bucket) const {
    deleteObjects(bucket, listObjects(bucket));
}

//----------------------------------------------------------------------------------------------------------------------

void S3ClientAWS::deleteBucket(const std::string& bucket) const {
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucket);

    auto outcome = client().DeleteBucket(request);

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

    return client().HeadBucket(request).IsSuccess();
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::listBuckets() const -> std::vector<std::string> {
    auto outcome = client().ListBuckets();

    if (outcome.IsSuccess()) {
        std::vector<std::string> buckets;
        for (const auto& bucket : outcome.GetResult().GetBuckets()) { buckets.emplace_back(bucket.GetName()); }
        return buckets;
    }

    auto msg = awsErrorMessage("Failed to list buckets!", outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// PUT OBJECT

auto S3ClientAWS::putObject(const S3ObjectPath& path, const void* buffer, const uint64_t length) const -> long long {
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(path.bucket);
    request.SetKey(path.object);
    // request.SetContentLength(length);

    if (buffer && length > 0) {
        auto streamBuffer = Aws::MakeShared<BufferIOStream>(allocTag, const_cast<void*>(buffer), length);
        request.SetBody(streamBuffer);
    } else {
        // empty object
        request.SetBody(Aws::MakeShared<Aws::StringStream>(allocTag));
    }

    auto outcome = client().PutObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Put " << path << " with len=" << length << '\n';
        /// @todo actual size of written bytes
        return length;
    }

    auto msg = awsErrorMessage("Failed to put " + std::string(path), outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// GET OBJECT

auto S3ClientAWS::getObject(const S3ObjectPath& path,
                            void*               buffer,
                            const uint64_t /*offset*/,
                            const uint64_t length) const -> long long {
    Aws::S3::Model::GetObjectRequest request;

    request.SetBucket(path.bucket);
    request.SetKey(path.object);
    request.SetResponseStreamFactory([&buffer, length]() { return Aws::New<BufferIOStream>(allocTag, buffer, length); });
    /// @todo range and streambuf
    // request.SetRange(std::to_string(offset) + "-" + std::to_string(offset + length));

    auto outcome = client().GetObject(request);

    if (outcome.IsSuccess()) {
        LOG_DEBUG_LIB(LibEcKit) << "Get " << path << '\n';
        LOG_DEBUG_LIB(LibEcKit) << "Req. len=" << length << ", Obj. len=" << outcome.GetResult().GetContentLength()
                                << '\n';
        return outcome.GetResult().GetContentLength();
    }

    auto msg = awsErrorMessage("Failed to retrieve " + std::string(path), outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------
// DELETE OBJECT

void S3ClientAWS::deleteObject(const S3ObjectPath& path) const {
    Aws::S3::Model::DeleteObjectRequest request;

    request.SetBucket(path.bucket);
    request.SetKey(path.object);

    auto outcome = client().DeleteObject(request);

    if (!outcome.IsSuccess()) {
        auto msg = awsErrorMessage("Failed to delete " + std::string(path), outcome.GetError());
        throw S3SeriousBug(msg, Here());
    }

    LOG_DEBUG_LIB(LibEcKit) << "Deleted " << path << '\n';
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

    auto outcome = client().DeleteObjects(request);

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

    auto outcome = client().ListObjectsV2(request);

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

auto S3ClientAWS::objectExists(const S3ObjectPath& path) const -> bool {
    Aws::S3::Model::HeadObjectRequest request;

    request.SetBucket(path.bucket);
    request.SetKey(path.object);

    return client().HeadObject(request).IsSuccess();
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ClientAWS::objectSize(const S3ObjectPath& path) const -> long long {
    Aws::S3::Model::HeadObjectRequest request;

    request.SetBucket(path.bucket);
    request.SetKey(path.object);

    auto outcome = client().HeadObject(request);

    if (outcome.IsSuccess()) { return outcome.GetResult().GetContentLength(); }

    const auto msg = awsErrorMessage("Object '" + path.object + "' doesn't exist or no access!", outcome.GetError());
    throw S3SeriousBug(msg, Here());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
