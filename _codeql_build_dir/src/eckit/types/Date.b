
Date(eckit::Bless& b):
	julian_(b(&julian_))
{
}

Date(eckit::Evolve b):
	julian_(b("Date","julian_"))
{
}

static const char* specName()      { return "Date"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Date));
	s.member("julian_",member_size(Date,julian_),member_offset(Date,julian_),"long");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"julian_",julian_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Date");
	eckit::_export(h,"julian_",julian_);
	eckit::_endClass(h,"Date");
}


