/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/config/Resource.h"
#include "eckit/web/Html.h"
#include "eckit/web/HtmlResource.h"
#include "eckit/web/Url.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// remember to add a mutex

HtmlResourceMap HtmlResource::resources_;

HtmlResource::HtmlResource(const std::string& s)
{
	resources_.init();
	(*resources_)[s] = this;
}

HtmlResource::~HtmlResource()
{
	// Should do something here...
}

void HtmlResource::dispatch(eckit::Stream& s,std::istream& in,std::ostream& out,Url& url)
{
	std::string str;

	for(int i = 0; i < url.size() ; i++)
	{
		str += "/" + url[i];


		HtmlResourceMap::iterator j = resources_->find(str);
		if(j != resources_->end())
		{
			HtmlResource *r = (*j).second;

			if(r->restricted() && !url.authenticated())
			{
				url.authenticate();
				return;
			}

			r->html(out,url);
			return;
		}		
	}

	url.status(Url::notFound);
	out << "Url not found: " << url << std::endl;


	std::string home = eckit::Resource<std::string>("homePage","http://hades.ecmwf.int/mars/");

	out << "Please, try the " << Html::Link(home) << "MARS home page" << Html::Link() << 
		'.' << std::endl;

}

void HtmlResource::index(std::ostream& s,Url& url)
{
	for(HtmlResourceMap::iterator j = resources_->begin(); j != resources_->end(); ++j)
	{
		s   << Html::Link((*j).first) << (*j).first << Html::Link() << std::endl;		
	}
}

//-----------------------------------------------------------------------------

} // namespace eckit


