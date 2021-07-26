/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <map>

#include "eckit/config/Resource.h"
#include "eckit/log/JSON.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/types/Types.h"
#include "eckit/web/Html.h"
#include "eckit/web/HttpResource.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Registry for all HttpResource's
///
class HttpResourceRegistry {

    typedef std::map<std::string, HttpResource*> HttpResourceMap;

public:  // methods
    /// Builds the registry on demand, needed for correct static initialization
    /// because factories can be initialized first
    static HttpResourceRegistry& instance() {
        static HttpResourceRegistry reg;
        return reg;
    }

    /// Registers an entry to the registry
    /// @pre Cannot exist yet
    /// @param obj pointer must be valid
    void enregister(const std::string& name, HttpResource* obj) {
        AutoLock<Mutex> lockme(mutex_);
        Log::debug() << "Registering http resource [" << name << "] to registry with address [" << obj << "]" << std::endl;
        ASSERT(obj);
        ASSERT(map_.find(name) == map_.end());
        map_[name] = obj;
    }

    /// Removes an entry from the registry
    /// @pre Must exist
    void deregister(const std::string& name) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(map_.find(name) != map_.end());
        map_.erase(name);
    }

    /// List entries in HttpResource
    std::vector<std::string> list() const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> result;
        for (HttpResourceMap::const_iterator j = map_.begin(); j != map_.end(); ++j) {
            result.push_back(j->first);
        }
        return result;
    }

    /// Check entry exists in registry
    bool exists(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);
        HttpResourceMap::const_iterator j = map_.find(name);
        return (j != map_.end());
    }

    /// Prints the entries in registry
    void print(std::ostream& out, const char* separator = ", ") const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> l = HttpResourceRegistry::instance().list();
        const char* sep            = "";
        for (auto j : l) {
            out << sep << j;
            sep = separator;
        }
    }

    /// Prints the index with the registry entries
    void index(std::ostream& s) const {
        AutoLock<Mutex> lockme(mutex_);
        for (HttpResourceMap::const_iterator j = map_.begin(); j != map_.end(); ++j) {
            s << Html::Link((*j).first) << (*j).first << Html::Link() << std::endl;
        }
    }

    /// Lookup entry in the registry
    /// @returns nullptr if registry not found
    HttpResource* lookup(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);
        HttpResource* r                   = nullptr;
        HttpResourceMap::const_iterator j = map_.find(name);
        if (j != map_.end()) {
            r = j->second;  // guarranteed not null
        }
        return r;
    }

protected:
    friend std::ostream& operator<<(std::ostream& s, const HttpResourceRegistry& r) {
        r.print(s);
        return s;
    }

private:  // members
    HttpResourceMap map_;
    mutable Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

HttpResource::HttpResource(const std::string& s) :
    resourceUrl_(s) {
    HttpResourceRegistry::instance().enregister(resourceUrl_, this);
}

HttpResource::~HttpResource() {
    HttpResourceRegistry::instance().deregister(resourceUrl_);
}

void HttpResource::print(std::ostream& s) const {
    s << "HttpResource[url=" << resourceUrl_ << "]";
}

static void error(Url& url, HttpStream& out, eckit::Exception& e, int code) {

    e.dumpStackTrace();

    out.print(Log::info());
    Log::info() << std::endl;

    out.reset();

    out.print(Log::info());
    Log::info() << std::endl;

    url.status(code, e.what());
    JSON json(out);
    json.startObject();
    json << "error" << e.what();
    json.endObject();

    out.print(Log::info());
    Log::info() << std::endl;
}

void HttpResource::dispatch(eckit::Stream&, std::istream&, HttpStream& out, Url& url) {

    std::string str;

    for (int i = 0; i < url.size(); i++) {
        str += "/" + url[i];

        HttpResource* r = HttpResourceRegistry::instance().lookup(str);

        if (r) {

            if (r->restricted() && !url.authenticated()) {
                url.authenticate();
                return;
            }

            std::vector<std::string> v;
            while (++i < url.size()) {
                v.push_back(url[i]);
            }
            url.remaining(v);

            Log::debug() << "HTTP resource [" << str << "] remaining " << v << std::endl;

            const std::string& method = url.method();

            try {

                if (method == "GET") {
                    r->GET(out, url);
                    return;
                }

                if (method == "HEAD") {
                    r->HEAD(out, url);
                    return;
                }

                if (method == "POST") {
                    r->POST(out, url);
                    return;
                }

                if (method == "PUT") {
                    r->PUT(out, url);
                    return;
                }

                if (method == "DELETE") {
                    r->DELETE(out, url);
                    return;
                }

                if (method == "TRACE") {
                    r->TRACE(out, url);
                    return;
                }

                if (method == "OPTIONS") {
                    r->OPTIONS(out, url);
                    return;
                }

                if (method == "CONNECT") {
                    r->CONNECT(out, url);
                    return;
                }

                if (method == "PATCH") {
                    r->PATCH(out, url);
                    return;
                }

                std::ostringstream oss;
                oss << "Unsupported HTTP method " << method << " url=" << url;
                throw eckit::MethodNotYetImplemented(oss.str());
            }
            catch (eckit::HttpError& e) {
                error(url, out, e, e.status());
            }
            catch (eckit::MethodNotYetImplemented& e) {
                error(url, out, e, HttpError::NOT_IMPLEMENTED);
            }
            catch (eckit::NotImplemented& e) {
                error(url, out, e, HttpError::NOT_IMPLEMENTED);
            }
            catch (eckit::UserError& e) {
                error(url, out, e, HttpError::BAD_REQUEST);
            }
            catch (eckit::Exception& e) {
                error(url, out, e, HttpError::INTERNAL_SERVER_ERROR);
            }
            return;
        }
    }

    url.status(HttpError::NOT_FOUND);
    out << "Url not found: " << url << std::endl;

    out << "Urls are:" << std::endl;

    index(out, url);
}

void HttpResource::index(std::ostream& s, Url&) {
    HttpResourceRegistry::instance().index(s);
}

const std::string& HttpResource::resourceUrl() const {
    return resourceUrl_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
