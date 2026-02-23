
Grid(eckit::Bless& b):
	northSouth_(b(&northSouth_)),
	eastWest_(b(&eastWest_))
{
}

Grid(eckit::Evolve b):
	northSouth_(b("Grid","northSouth_")),
	eastWest_(b("Grid","eastWest_"))
{
}

static const char* specName()      { return "Grid"; }
static void isa(TypeInfo* t)  { eckit::Isa::add(t,specName()); }
static eckit::Isa* isa()             { return eckit::Isa::get(specName());  }

static void schema(eckit::Schema& s)
{
	s.start(specName(),sizeof(Grid));
	s.member("northSouth_",member_size(Grid,northSouth_),member_offset(Grid,northSouth_),"double");
	s.member("eastWest_",member_size(Grid,eastWest_),member_offset(Grid,eastWest_),"double");
	s.end(specName());
}


void describe(std::ostream& s,int depth = 0) const {
	eckit::_startClass(s,depth,specName());
	eckit::_describe(s,depth+1,"northSouth_",northSouth_);
	eckit::_describe(s,depth+1,"eastWest_",eastWest_);
	eckit::_endClass(s,depth,specName());
}



void _export(eckit::Exporter& h) const {
	eckit::_startClass(h,"Grid");
	eckit::_export(h,"northSouth_",northSouth_);
	eckit::_export(h,"eastWest_",eastWest_);
	eckit::_endClass(h,"Grid");
}


