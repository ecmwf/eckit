/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/web/HtmlResource.h"
#include "eckit/config/Resource.h"
#include "eckit/parser/JSON.h"
#include "eckit/types/Types.h"
#include "eckit/web/Html.h"
#include "eckit/web/Url.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// remember to add a mutex

HtmlResourceMap HtmlResource::resources_;

HtmlResource::HtmlResource(const std::string& s) : resourceUrl_(s) {
    resources_.init();
    (*resources_)[s] = this;
}

HtmlResource::~HtmlResource() {
    // Should do something here...
}

void HtmlResource::dispatch(eckit::Stream& s, std::istream& in, std::ostream& out, Url& url) {
    std::string str;

    for (int i = 0; i < url.size(); i++) {
        str += "/" + url[i];


        HtmlResourceMap::iterator j = resources_->find(str);
        if (j != resources_->end()) {
            HtmlResource* r = (*j).second;

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
                url.status(e.status(), e.what());
                JSON json(out);
                json.startObject();
                json << "error" << e.what();
                json.endObject();
            }
            catch (eckit::MethodNotYetImplemented& e) {
                url.status(HttpError::NOT_IMPLEMENTED, e.what());
                JSON json(out);
                json.startObject();
                json << "error" << e.what();
                json.endObject();
            }
            catch (eckit::NotImplemented& e) {
                url.status(HttpError::NOT_IMPLEMENTED, e.what());
                JSON json(out);
                json.startObject();
                json << "error" << e.what();
                json.endObject();
            }
            catch (eckit::UserError& e) {
                url.status(HttpError::BAD_REQUEST, e.what());
                JSON json(out);
                json.startObject();
                json << "error" << e.what();
                json.endObject();
            }
            catch (eckit::Exception& e) {
                url.status(HttpError::INTERNAL_SERVER_ERROR, e.what());
                JSON json(out);
                json.startObject();
                json << "error" << e.what();
                json.endObject();
            }
            return;
        }
    }

    url.status(Url::notFound);
    out << "Url not found: " << url << std::endl;

    out << "Urls are:" << std::endl;

    index(out, url);
}

void HtmlResource::index(std::ostream& s, Url& url) {
    for (HtmlResourceMap::iterator j = resources_->begin(); j != resources_->end(); ++j) {
        s << Html::Link((*j).first) << (*j).first << Html::Link() << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
