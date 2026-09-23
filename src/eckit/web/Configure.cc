// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @note This code is here to avoid linking it in if you don't need it

#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/log/Log.h"
#include "eckit/web/Html.h"
#include "eckit/web/HtmlResource.h"
#include "eckit/web/Url.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void Configurable::htmlAllResources(std::ostream& s, Url& url) {
    callAll(&Configurable::htmlResources, s, url);
}

void Configurable::htmlResources(std::ostream& s, Url& url) {
    for (Set::const_iterator i = resources_.begin(); i != resources_.end(); ++i) {
        (*i)->GET(s, url);
    }
}

void ResourceBase::GET(std::ostream& s, Url& url) {

    std::string n = name();
    std::string u = url["name"];

    init();

    if (n == u) {
        std::string v = url[n];
        Log::info() << "New value for " << n << ": " << v << std::endl;
        ResourceMgr::instance().set(n, v);
        Configurable::reconfigureAll();
    }

    dump(s);

    s << Html::BeginForm();
    s << Html::TextField(n, getValue(), n + ": ");
    s << Html::SubmitButton();
    s << Html::ResetButton();
    s << Html::HiddenField("name", n);
    s << Html::EndForm();
    s << Html::Line();
}


//----------------------------------------------------------------------------------------------------------------------

class ConfigResource : public HtmlResource {
    virtual bool restricted() { return true; }
    virtual void GET(std::ostream&, Url&);

public:

    ConfigResource() : HtmlResource("/config") {}
};

void ConfigResource::GET(std::ostream& s, Url& url) {
    Configurable::htmlAllResources(s, url);
}

static ConfigResource ostoreResource;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
