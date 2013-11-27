/*
 * (C) Copyright 1996-2013 ECMWF.
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

HtmlResource::Map* HtmlResource::resources_ = 0;

HtmlResource::HtmlResource(const string& s)
{
	if(!resources_) resources_ = new Map();
	(*resources_)[s] = this;
}

HtmlResource::~HtmlResource()
{
	// Should do something here...
}

void HtmlResource::dispatch(eckit::Stream& s,std::istream& in,std::ostream& out,Url& url)
{
	Map* m = resources_;

	string str;

	for(int i = 0; i < url.size() ; i++)
	{
		str += "/" + url[i];


		Map::iterator j = m->find(str);
		if(j != m->end())
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


	string home = eckit::Resource<string>("homePage","http://hades.ecmwf.int/mars/");

	out << "Please, try the " << Html::Link(home) << "MARS home page" << Html::Link() << 
		'.' << std::endl;

}

void HtmlResource::index(std::ostream& s,Url& url)
{
	for(Map::iterator j = resources_->begin(); j != resources_->end(); ++j)
	{
		s   << Html::Link((*j).first) << (*j).first << Html::Link() << std::endl;		
	}
}

//-----------------------------------------------------------------------------

} // namespace eckit


