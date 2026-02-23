
DateTime(eckit::Bless& b):
	date_(b(&date_)),
	time_(b(&time_))
{
}

DateTime(eckit::Evolve b):
	date_(b("DateTime","date_")),
	time_(b("DateTime","time_"))
{
}

static const char* specName()      { return "DateTime"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(DateTime));
	s.member("date_",member_size(DateTime,date_),member_offset(DateTime,date_),"Date");
	s.member("time_",member_size(DateTime,time_),member_offset(DateTime,time_),"Time");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"date_",date_);
	eckit::_describe(s,depth+1,"time_",time_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"DateTime");
	eckit::_export(h,"date_",date_);
	eckit::_export(h,"time_",time_);
	eckit::_endClass(h,"DateTime");
}


