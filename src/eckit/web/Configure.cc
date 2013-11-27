/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @note This code is here to avoid linking it in if you don't need it

#include "eckit/config/Configurable.h"
#include "eckit/log/Log.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"

#include "eckit/web/Html.h"
#include "eckit/web/HtmlResource.h"
#include "eckit/web/Url.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void Configurable::htmlAllResources(std::ostream& s,Url& url) 
{ 
	callAll(&Configurable::htmlResources,s,url);
}

void Configurable::htmlResources(std::ostream& s,Url& url)
{
	for(Set::const_iterator i=resources_.begin();
		i != resources_.end();++i)
		(*i)->html(s,url);
}

void ResourceBase::html(std::ostream& s,Url& url)
{

	string n = name();
	string u = url["name"];

	init();

	if( n == u)
	{
		string v = url[n];
		Log::info() << "New value for " << n << ": " << v << std::endl;
		ResourceMgr::instance().set(n,v);
		Configurable::reconfigureAll();
	}

	dump(s);

	s << Html::BeginForm();
	s << Html::TextField(n,getValue(),n + ": ");
	s << Html::SubmitButton();
	s << Html::ResetButton();
	s << Html::HiddenField("name",n);
	s << Html::EndForm();
	s << Html::Line();
}


//=================================================================

class ConfigResource : public HtmlResource {
	virtual bool restricted() { return true; }
	virtual void html(std::ostream&,Url&);	
public:
	ConfigResource() : HtmlResource("/config") {}	
};

void ConfigResource::html(std::ostream& s,Url& url)
{
	Configurable::htmlAllResources(s,url);
}

static ConfigResource ostoreResource;

//-----------------------------------------------------------------------------

} // namespace eckit

