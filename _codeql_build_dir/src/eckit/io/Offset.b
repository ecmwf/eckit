
Offset(eckit::Bless& b):
	value_(b(&value_))
{
}

Offset(eckit::Evolve b):
	value_(b("Offset","value_"))
{
}

static const char* specName()      { return "Offset"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Offset));
	s.member("value_",member_size(Offset,value_),member_offset(Offset,value_),"value_t");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"value_",value_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Offset");
	eckit::_export(h,"value_",value_);
	eckit::_endClass(h,"Offset");
}


