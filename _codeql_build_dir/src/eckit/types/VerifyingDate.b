
VerifyingDate(eckit::Bless& b):
	DateTime(b)
{
}

VerifyingDate(eckit::Evolve b):
	DateTime(b("VerifyingDate"))
{
}

static const char* specName()      { return "VerifyingDate"; }
static void isa(TypeInfo* t)  { DateTime::isa(t);eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(VerifyingDate));
	DateTime::schema(s);
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	DateTime::describe(s,depth+1);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"VerifyingDate");
	DateTime::_export(h);
	eckit::_endClass(h,"VerifyingDate");
}


