/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/marsfs/MarsFSClient.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/thread/ThreadSingleton.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

//#define X(a) Log::userInfo() << #a << std::endl; Log::info() << #a << std::endl;
#define X(a) /**/

// TODO: Return errno
//

class MarsFSClientSettings {
    bool forever_;

public:
    MarsFSClientSettings() : forever_(true) {}

    bool retry() const { return forever_; }

    void retry(bool on) { forever_ = on; }
};

static ThreadSingleton<MarsFSClientSettings> settings;

inline static bool retry() {
    return settings.instance().retry();
}

MarsFSClientRetry::MarsFSClientRetry(bool on) : old_(settings.instance().retry()) {
    settings.instance().retry(on);
}

MarsFSClientRetry::~MarsFSClientRetry() {
    settings.instance().retry(old_);
}

MarsFSClient::MarsFSClient(const MarsFSPath& path) : connector_(net::Connector::service("marsfs", path.node())) {}

MarsFSClient::~MarsFSClient() {}

Length MarsFSClient::size(const std::string& path) {
    X(MarsFSClient::size);

    for (;;) {

        try {
            net::AutoMemoize m(connector_, 1);

            Stream& s = connector_;
            unsigned long long len;
            s << "size";
            s << path;
            s >> len;
            return len;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

bool MarsFSClient::exists(const std::string& path) {
    X(MarsFSClient::exit);

    for (;;) {

        try {
            // net::AutoMemoize m(connector_, 10);

            bool ok;
            Stream& s = connector_;

            s << "exists";
            s << path;
            s >> ok;
            return ok;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::string MarsFSClient::mountPoint(const std::string& path) {
    X(MarsFSClient::mountPoint);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 60);

            std::string p;
            Stream& s = connector_;
            s << "mountp";
            s << path;
            s >> p;
            return p;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::string MarsFSClient::baseName(const std::string& path, bool ext) {
    X(MarsFSClient::baseName);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 60);

            std::string b;
            Stream& s = connector_;
            s << "baseName";
            s << path;
            s << ext;
            s >> b;
            return b;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::string MarsFSClient::dirName(const std::string& path) {
    X(MarsFSClient::dirName);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 60);

            std::string d;
            Stream& s = connector_;
            s << "dirName";
            s << path;
            s >> d;
            return d;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::string MarsFSClient::fullName(const std::string& path) {
    X(MarsFSClient::fullName);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 60);

            std::string f;
            Stream& s = connector_;
            s << "fullName";
            s << path;
            s >> f;
            return f;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::string MarsFSClient::unique(const std::string& path) {
    X(MarsFSClient::unique);

    for (;;) {
        try {
            std::string u;
            Stream& s = connector_;
            s << "unique";
            s << path;
            s >> u;
            return u;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

bool MarsFSClient::sameAs(const std::string& path1, const std::string& path2) {
    X(MarsFSClient::sameAs);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 10);

            bool ok;
            Stream& s = connector_;
            s << "sameas";
            s << path1;
            s << path2;
            s >> ok;
            return ok;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::mkdir(const std::string& path, short mode) {
    X(MarsFSClient::mkdir);

    for (;;) {
        try {
            bool ok;
            Stream& s = connector_;
            s << "mkdir";
            s << path;
            s << mode;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::match(const std::string& path, std::vector<std::string>& result, bool recurse) {
    X(MarsFSClient::match);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 2);

            Stream& s = connector_;
            s << "match";
            s << path;
            s << recurse;

            result.clear();

            long n;
            s >> n;

            std::string r;

            for (long i = 0; i < n; i++) {
                s >> r;
                result.push_back(r);
            }
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::children(const std::string& path, std::vector<std::string>& files, std::vector<std::string>& dirs) {
    X(MarsFSClient::children);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 2);

            Stream& s = connector_;
            s << "children";
            s << path;

            std::string r;
            long n;

            /// @note follow the children() signature order, send files first then dirs

            files.clear();
            s >> n;
            for (long i = 0; i < n; i++) {
                s >> r;
                files.push_back(r);
            }

            dirs.clear();
            s >> n;
            for (long i = 0; i < n; i++) {
                s >> r;
                dirs.push_back(r);
            }

            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::vector<std::string> MarsFSClient::getFileSpaces() {
    X(MarsFSClient::getFileSpaces);
    for (;;) {
        try {

            net::AutoMemoize m(connector_, 20);

            Stream& s = connector_;
            s << "getFileSpaces";

            std::vector<std::string> result;

            long n;
            s >> n;

            std::string r;

            for (long i = 0; i < n; i++) {
                s >> r;
                result.push_back(r);
            }

            return result;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

std::vector<std::string> MarsFSClient::getFileSystems(const std::string& name) {
    X(MarsFSClient::getFileSystems);

    for (;;) {
        try {
            net::AutoMemoize m(connector_, 20);

            Stream& s = connector_;
            s << "getFileSystems";
            s << name;

            std::vector<std::string> result;

            long n;
            s >> n;

            std::string r;

            for (long i = 0; i < n; i++) {
                s >> r;
                result.push_back(r);
            }

            return result;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::rename(const std::string& from, const std::string& to) {
    X(MarsFSClient::rename);
    for (;;) {
        try {
            bool ok;
            Stream& s = connector_;
            s << "rename";
            s << from;
            s << to;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::link(const std::string& from, const std::string& to) {
    X(MarsFSClient::link);
    for (;;) {
        try {

            bool ok;
            Stream& s = connector_;
            s << "link";
            s << from;
            s << to;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::unlink(const std::string& path) {
    X(MarsFSClient::unlink);
    for (;;) {
        try {

            bool ok;
            Stream& s = connector_;
            s << "unlink";
            s << path;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::syncParentDirectory(const std::string& path) {
    X(MarsFSClient::syncParentDirectory);
    for (;;) {
        try {

            bool ok;
            Stream& s = connector_;
            s << "syncParentDirectory";
            s << path;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::rmdir(const std::string& path) {
    X(MarsFSClient::rmdir);
    for (;;) {

        try {

            bool ok;
            Stream& s = connector_;
            s << "rmdir";
            s << path;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::touch(const std::string& path) {
    X(MarsFSClient::touch);
    for (;;) {

        try {

            bool ok;
            Stream& s = connector_;
            s << "touch";
            s << path;
            s >> ok;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

void MarsFSClient::fileSystemSize(const std::string& path, FileSystemSize& fs) {
    X(MarsFSClient::fileSystemSize);
    for (;;) {
        try {
            net::AutoMemoize m(connector_, 10);

            Stream& s = connector_;
            s << "statfs";
            s << path;

            s >> fs.available;
            s >> fs.total;
            return;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

time_t MarsFSClient::created(const std::string& path) {
    X(MarsFSClient::created);
    for (;;) {

        try {

            net::AutoMemoize m(connector_, 60);

            Stream& s = connector_;
            unsigned long long len;
            s << "created";
            s << path;
            s >> len;
            return len;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

time_t MarsFSClient::lastAccess(const std::string& path) {
    X(MarsFSClient::lastAccess);
    for (;;) {

        try {

            net::AutoMemoize m(connector_, 1);

            Stream& s = connector_;
            unsigned long long len;
            s << "lastAccess";
            s << path;
            s >> len;
            return len;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

time_t MarsFSClient::lastModified(const std::string& path) {
    X(MarsFSClient::lastModified);
    for (;;) {

        try {

            net::AutoMemoize m(connector_, 1);

            Stream& s = connector_;
            unsigned long long len;
            s << "lastModified";
            s << path;
            s >> len;
            return len;
        }
        catch (net::ConnectorException& e) {
            if (!retry())
                throw;
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
