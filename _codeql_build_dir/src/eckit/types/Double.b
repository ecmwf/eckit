
Double(eckit::Bless& b):
	value_(b(&value_))
{
}

Double(eckit::Evolve b):
	value_(b("Double","value_"))
{
}

static const char* specName()      { return "Double"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Double));
	s.member("value_",member_size(Double,value_),member_offset(Double,value_),"double");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"value_",value_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Double");
	eckit::_export(h,"value_",value_);
	eckit::_endClass(h,"Double");
}


