
#include "eckit/filesystem/URI.h"
#include "eckit/io/s3/S3URIManager.h"
#include "eckit/io/s3/S3Name.h"

namespace eckit {

static S3URIManager manager_s3("s3");

//----------------------------------------------------------------------------------------------------------------------

S3URIManager::S3URIManager(const std::string& name) :
    URIManager(name) {}

S3URIManager::~S3URIManager() = default;

bool S3URIManager::exists(const URI& uri) {
    return S3Name(uri).exists();
}

DataHandle* S3URIManager::newWriteHandle(const URI& uri) {
    return S3Name(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri) {
    return S3Name(uri).dataHandle();
}

DataHandle* S3URIManager::newReadHandle(const URI& uri, const OffsetList&, const LengthList&) {
    return S3Name(uri).dataHandle();
}

std::string S3URIManager::asString(const URI& uri) const {
    return std::string("s3://") + uri.name();
}

//----------------------------------------------------------------------------------------------------------------------

}

