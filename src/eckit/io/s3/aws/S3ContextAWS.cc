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

#include "eckit/io/s3/aws/S3ContextAWS.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/io/s3/S3Exception.h"
#include "eckit/io/s3/aws/S3ClientAWS.h"

#include <aws/core/auth/AWSCredentialsProviderChain.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

S3ContextAWS::S3ContextAWS(const Aws::SDKOptions& options): S3Context(S3Types::AWS), options_(options) {
    LOG_DEBUG_LIB(LibEcKit) << "Initialize AWS API!" << std::endl;
    Aws::InitAPI(options_);
}

S3ContextAWS::~S3ContextAWS() {
    LOG_DEBUG_LIB(LibEcKit) << "Shutdown AWS API!" << std::endl;
    Aws::ShutdownAPI(options_);
}

//----------------------------------------------------------------------------------------------------------------------

auto S3ContextAWS::makeShared() -> std::shared_ptr<S3Context> {
    Aws::SDKOptions options;
    // TODO: remove debugging logs
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    return std::make_shared<S3ContextAWS>(options);
}

///>>> Where are you anticipating getting credentials from?

auto S3ContextAWS::checkCredentials(const char* chain) -> bool {
    // AWS S3 permits unauthenticated requests; client returns "success" but 0 buckets
    auto provider = Aws::MakeShared<Aws::Auth::DefaultAWSCredentialsProviderChain>(chain);

    auto creds = provider->GetAWSCredentials();
    if (creds.IsExpiredOrEmpty()) {
        Log::warning() << "Authentication failed!" << std::endl;
        return false;
    }

    Log::info() << "AWS authentication was successful!" << std::endl;

    return true;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
