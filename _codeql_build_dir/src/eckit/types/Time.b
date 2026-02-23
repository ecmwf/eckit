
Time(eckit::Bless& b):
	seconds_(b(&seconds_))
{
}

Time(eckit::Evolve b):
	seconds_(b("Time","seconds_"))
{
}

static const char* specName()      { return "Time"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Time));
	s.member("seconds_",member_size(Time,seconds_),member_offset(Time,seconds_),"Second");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"seconds_",seconds_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Time");
	eckit::_export(h,"seconds_",seconds_);
	eckit::_endClass(h,"Time");
}


