
BadTime(eckit::Bless& b):
	BadValue(b)
{
}

BadTime(eckit::Evolve b):
	BadValue(b("BadTime"))
{
}

static const char* specName()      { return "BadTime"; }
static void isa(TypeInfo* t)  { BadValue::isa(t);eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(BadTime));
	BadValue::schema(s);
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	BadValue::describe(s,depth+1);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"BadTime");
	BadValue::_export(h);
	eckit::_endClass(h,"BadTime");
}


