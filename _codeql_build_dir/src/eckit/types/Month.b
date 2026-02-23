
Month(eckit::Bless& b):
	date_(b(&date_))
{
}

Month(eckit::Evolve b):
	date_(b("Month","date_"))
{
}

static const char* specName()      { return "Month"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Month));
	s.member("date_",member_size(Month,date_),member_offset(Month,date_),"Date");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"date_",date_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Month");
	eckit::_export(h,"date_",date_);
	eckit::_endClass(h,"Month");
}


