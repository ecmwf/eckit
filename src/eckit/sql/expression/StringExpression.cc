/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression//ColumnExpression.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/expression/StringExpression.h"
#include "eckit/utils/Regex.h"


//----------------------------------------------------------------------------------------------------------------------

namespace {

bool isColumnRegex(const std::string& s)
{
    return ( s[0] == '/' && s[s.size() - 1] == '/' )
        || ( s[0] == '~' && s[1] == '/' && s[s.size() - 1] == '/' );
}

bool matchEx(const std::string& regex, const std::string& s)
{
    bool negated = false;
    std::string rx = regex;
    if (rx[0] == '~')
    {
        rx.erase(0, 0);
        negated = true;
    }

    // TODO: remove '/'

    bool matches = eckit::Regex(rx).match(s);
    return (negated && !matches) || (!negated && matches);
}

}

namespace eckit {
namespace sql {
namespace expression {


//----------------------------------------------------------------------------------------------------------------------

const eckit::sql::type::SQLType* StringExpression::type() const { return &eckit::sql::type::SQLType::lookup("string"); }

StringExpression::StringExpression(const std::string& name)
: name_(name)
{
	ASSERT(name.length() <= sizeof(value_));

	char* buf = (char*)&value_;
	memset(buf,' ',sizeof(value_));

	int  off = sizeof(value_) - name.length();
	for(size_t i = 0; i < name.length(); i++)
		buf[off+i] = name[i];

	std::string s(buf, sizeof(double));
	//Log::info() << "StringExpression::StringExpression: '" << s << "'" << std::endl;
}

StringExpression::StringExpression(const StringExpression& o)
: name_(o.name_), value_(o.value_)
{}

void StringExpression::expandStars(const std::vector<std::reference_wrapper<const SQLTable>>& tables, expression::Expressions& e)
{
    std::ostream& L(Log::info());

    //TODO: if(verbose_) {...}
    //Log::info() << "StringExpression::expandStars: name_: '"  << name_ << "', value_: '" << value_ << "'" << std::endl;

    if (!isColumnRegex(name_))
	{
        e.push_back(shared_from_this());
		return;
	}

	unsigned int matched = 0;
    for(const auto& table : tables) {
        std::vector<std::string> names = table.get().columnNames();

		for(size_t i = 0; i < names.size(); i++)
		{
			const std::string& name = names[i];
            if (!matchEx(name_, name))
			{
				L << "StringExpression::expandStars: skip '" << name << "'" << std::endl;
				continue;
			}
			
			L << "StringExpression::expandStars: adding '" << name << "'" << std::endl;
			++matched;
            e.push_back(std::make_shared<ColumnExpression>(name, &table.get()));
		}
	}
	if (! matched)
		throw eckit::UserError(std::string("No columns matching regex '") + name_ + "' found.");
}

std::shared_ptr<SQLExpression> StringExpression::clone() const { return std::make_shared<StringExpression>(name_); }

StringExpression::~StringExpression() {}

double StringExpression::eval(bool& missing) const { return value_; }

void StringExpression::prepare(SQLSelect& sql) {}

void StringExpression::cleanup(SQLSelect& sql) {}

void StringExpression::output(std::ostream& s) const { s << name_; }

void StringExpression::print(std::ostream& s) const { s << "'" << name_ << "'"; }

//----------------------------------------------------------------------------------------------------------------------

} // namespace expression
} // namespace sql
} // namespace eckit
