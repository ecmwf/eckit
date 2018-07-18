/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/Tokenizer.h"

#include "odb_api/DataColumns.h"
#include "odb_api/DataTable.h"

#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLDataTable.h"
#include "eckit/sql/type/SQLType.h"

using namespace eckit;

namespace eckit {
namespace sql {

void SQLDatabase::setUpVariablesTable()
{
	tablesByName_["dual"] = dualTable();
}

SQLDatabase::SQLDatabase(const PathName& path,const std::string& name)
: path_(path),
  name_(name),
  dualTable_(0),
  dual_(0)
{
	setUpVariablesTable();
} 

SQLDatabase::SQLDatabase(const std::string& name)
: path_("."), 
  name_(name),
  dualTable_(0),
  dual_(0)
{
	setUpVariablesTable();
} 

SQLDatabase::~SQLDatabase()
{
	close();
	for (Variables::iterator it = variables_.begin(); it != variables_.end(); ++it)
	{
	
		//std::string var(it->first);
		//cout << "SQLDatabase::~SQLDatabase: " <<  var << std::endl;
		//SQLExpression* e (it->second);
		//if (e->isVector())
		//	std::cout << var << " = " << *e << std::endl;
		//else
		//{
		//	bool missing = false;
		//	double value(it->second->eval(missing));
		//	std::cout << var << " = " << value << std::endl;
		//}
		//FIXME:
		//delete it->second;
	}
	variables_.clear();
    // TODO: FIXME
    //delete dual_;
    //delete dualTable_;
}

void SQLDatabase::open()
{
	Log::info() << "Opening " << path_ << " as " << name_ << std::endl;
#if 0
	loadDD();
	loadIOMAP();
	loadFLAGS();
#endif
}

void SQLDatabase::close()
{
    for(std::map<std::string,SQLTable*>::iterator j = tablesByName_.begin();
        j != tablesByName_.end(); ++j)
    {
        SQLTable* table = (*j).second;
        delete table;
    }

    tablesByName_.clear();
}


void SQLDatabase::addTable(SQLTable* table) {
    tablesByName_[table->name()] = table;
}

#if 0
void SQLDatabase::loadFLAGS()
{
	std::string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".flags";
	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	Tokenizer parse("=(),");
	char line[1024];
	while(in.getline(line,sizeof(line)-1))
	{
		if(line[0] == '-' && line[1] == 'A')
		{
			std::string s(line+2);
			std::vector<std::string> v;
			parse(s,v);

			if(tablesByName_.find(v[0]) == tablesByName_.end())
			{
				Log::warning() << path << ": Cannot find master table '" << v[0] << "'" << std::endl;
			}
			else
			{
				SQLTable* master = tablesByName_[v[0]];

				for(int i = 1 ; i < v.size(); i++)
				{
					if(tablesByName_.find(v[i]) == tablesByName_.end())
						Log::warning() << path << ": Cannot find slave table '" 
							<< v[i] << "' of '" << master->name() 
							<< "'" << std::endl;
					else					
						tablesByName_[v[i]]->master(master);
				}
			}
			
		}
	}
}

void SQLDatabase::loadDD()
{
	std::string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".dd";

	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	Tokenizer parse(":@");
	Tokenizer braket("()");

	std::string junk;
	std::string s;

	//cout << path << std::endl;

	in >> junk >> junk >> junk; // version ?
	in >> junk >> junk ; // dates ?
	in >> junk >> junk ; // dates ?
	in >> junk >> junk ; // dates ?

	int no_pools = 0,no_tables = 0;

	in >> no_pools;
	in >> no_tables;

	//cout << "no_pools " << no_pools << std::endl;
	//cout << "no_tbales " << no_tables << std::endl;

	for(int i = 0; i < no_tables; i++)
	{
		int id; std::string name;
		int n;
		in >> id >> name >> n;
		while(n-->0) in >> junk;
	}

	std::map<std::string,set<std::string> > links;

	for(int i = 0; i < no_tables; i++)
	{
		std::string name; int no_cols;
		in >> name >> no_cols; name.erase(0,1);
		ASSERT(tablesByName_.find(name) == tablesByName_.end());

		SQLTable* table = new SQLTable(*this,path_,name,no_cols,i);
		tablesByName_[name] = table;


		for(int j = 0; j < no_cols; j++)
		{
			std::string type; int no_fields;
			in >> type >> no_fields;

			//cout << type << " " << no_fields << std::endl;

			std::vector<std::string> v;
			parse(type,v);

			//cout << name << " " << v[0] << " " << v[1] << std::endl;
			std::vector<std::string> b;
			braket(v[1],b);

			if(b.size() > 1)
			{
				if(b[0] == "LINKOFFSET") { v[1] = b[1] + "." + "offset"; }
				if(b[0] == "LINKLEN")    { v[1] = b[1] + "." + "length"; }

				links[name].insert(b[1]);
			}

			std::vector<std::string> bitmap;

			if(no_fields)
			{
				std::vector<std::string> fields;
				std::vector<int>    sizes;
		
				for(int k = 0; k < no_fields; k++)
				{
					std::string name; int size;
					in >> name >> size;

					fields.push_back(name);
					sizes.push_back(size);
				//cout <<  name << " " << size << std::endl;
					// table->addBitColumn(...)
					bitmap.push_back(name);
				}
				v[0] = SQLBitfield::make(v[0],fields,sizes);
			}

			table->addColumn(v[1], j,SQLType::lookup(v[0]), bitmap);
		}
	}

	int no_variables;
	in >> no_variables;

	for(int i = 0; i < no_variables; i++)
	{
		std::string name; double value;
		in >> name >> value;
		variables_[name] = value;

		//Log::info() << "Variable " << name << " = " << value << std::endl;
	}
	setLinks(links);
}
#endif

void SQLDatabase::setLinks(const Links& links)
{
	for(Links::const_iterator j = links.begin(); j != links.end() ; ++j)
	{
		const std::string&      from = (*j).first;
        const std::set<std::string>& to   = (*j).second;

		ASSERT(tablesByName_.find(from) != tablesByName_.end());
		SQLTable *f = tablesByName_[from];

        for(std::set<std::string>::const_iterator k = to.begin(); k != to.end() ; ++k)
		{
			ASSERT(tablesByName_.find(*k) != tablesByName_.end());
			SQLTable* t = tablesByName_[*k];

			f->addLinkTo(t);
			t->addLinkFrom(f);
		}
	}
}

#if 0
void SQLDatabase::loadIOMAP()
{
	std::string name   = path_.baseName(false);
	PathName path = path_ + "/" + name + ".iomap";

	ifstream in(path.c_str());
	if(!in) throw CantOpenFile(path);

	int junk;
	std::string s;
	std::string table;

	int no_tables,no_pools;

	in >> junk;
	in >> s;

	in >> no_tables >> no_pools >> junk;
	in >> junk >> junk >> junk;

	for(;;)
	{
		std::string name; int id, n_cols;

		in >> id >> n_cols >> name; name.erase(0,1);
		if(id == -1) break;

		ASSERT(tablesByName_.find(name) != tablesByName_.end());
		tablesByName_[name]->loadIOMAP(in);

	}
}
#endif

SQLTable* SQLDatabase::defaultTable()
{
    std::map<std::string,SQLTable*>::iterator j (tablesByName_.find("defaultTable"));
    if (j == tablesByName_.end())
        throw UserError("No default table");
	return (*j).second;
}

SQLTable* SQLDatabase::table(const Table& t)
{
	std::map<std::string,SQLTable*>::iterator j (tablesByName_.find(t.name));
	ASSERT(j != tablesByName_.end());
	return (*j).second;
}

void SQLDatabase::setVariable(const std::string& name, expression::SQLExpression* value) {
	variables_[name] = value;
}

expression::SQLExpression* SQLDatabase::getVariable(const std::string& name) const
{
	Variables::const_iterator j = variables_.find(name);
	if(j == variables_.end())
		throw eckit::UserError("Undefined variable", name);
	return (*j).second;
}

void SQLDatabase::setIncludePath(const std::string& includePath)
{
    Tokenizer tokenize(":");
    std::vector<std::string> tokens;
    tokenize(includePath, tokens);
    copy(tokens.begin(), tokens.end(), back_inserter(includePath_));
}

bool SQLDatabase::sameAs(const SQLDatabase& other) const
{
	return path_ == other.path_;
}

} // namespace sql
} // namespace eckit
