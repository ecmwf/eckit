/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JSON.h
// Baudouin Raoult - (c) ECMWF Jun 12

#include <eclib/JSONParser.h>
#include <eclib/Translator.h>

class JSONTokenizerError : public exception {
    string what_;
    virtual const char* what() const  throw()
    {
        return what_.c_str();
    }
public:
    JSONTokenizerError(const string& what) : what_(what) {}
    virtual ~JSONTokenizerError() throw() {}
};


char JSONParser::peek()
{ 
    char c;
    for(;;)
    {
        c = in_.peek();
        if(in_.eof())
            return 0;
        if(!isspace(c))
        {
            //cout << "peek(" << c << ")" << endl;
            return c;
        }
        else {
            //cout << "skip(" << c << ")" << endl;
            in_.get(c);
        }
    }
}

char JSONParser::next(bool spaces)
{
    char c;
    for(;;)
    {
        if(in_.eof())
            throw JSONTokenizerError(string("JSONTokenizer::next reached eof"));
        in_.get(c);
        if(spaces || !isspace(c))
        {
            //cout << "next(" << c << ")" << endl;
            return c;
        }
    }

}

void JSONParser::consume(char c)
{
    char n = next();
    if(c != n)
        throw JSONTokenizerError(string("JSONTokenizer::consume expecting '") + c + "', got '" + n + "'");
}

void JSONParser::consume(const char* p)
{
    while(*p) consume(*p++);
}

Value JSONParser::parseTrue()
{
    consume("true");
    return Value(true);
}

Value JSONParser::parseFalse()
{
    consume("false");
    return Value(false);
}

Value JSONParser::parseNull()
{
    consume("null");
    return Value();
}

Value JSONParser::parseNumber()
{
    bool real = false;
    string s;
    char c = next();
    if(c == '-') {
        s += c;
        c = next();
    }

    switch(c) {
    case '0': s += c; break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        s += c;
        while(isdigit(peek())) {
            s += next();
        }
        break;
    default:
        throw JSONTokenizerError(string("JSONTokenizer::parseNumber invalid char '") + c + "'");
        break;
    }

    if(peek() == '.') {
        real = true;
        s += next();
        c = next();
        if(!isdigit(c))
            throw JSONTokenizerError(string("JSONTokenizer::parseNumber invalid char '") + c + "'");
        s += c;
        while(isdigit(peek())) {
            s += next();
        }
    }


    c = peek();
    if(c == 'e' || c == 'E') {
        real = true;
        s += next();

        c = next();
        if(c == '-' || c == '+')
        {
            s += c;
            c = next();
        }

        if(!isdigit(c))
            throw JSONTokenizerError(string("JSONTokenizer::parseNumber invalid char '") + c + "'");
        s += c;
        while(isdigit(peek())) {
            s += next();
        }

    }

    if(real) {
        double d = Translator<string,double>()(s);
        return Value(d);
    }
    else
    {
        long long d = Translator<string,long long>()(s);
        return Value(d);
    }
}

Value JSONParser::parseString()
{
    consume('"');
    string s;
    for(;;)
    {
        char c = next(true);
        if(c == '\\')
        {
            c = next(true);
            switch(c) {

            case '"':
                s += '"';
                break;

            case '\\':
                s += '\\';
                break;

            case '/':
                s += '/';
                break;

            case 'b':
                s += '\b';
                break;

            case 'f':
                s += '\f';
                break;

            case 'n':
                s += '\n';
                break;

            case 'r':
                s += '\r';
                break;

            case 't':
                s += '\t';
                break;

            case 'u':
                throw JSONTokenizerError(string("JSONTokenizer::parseString \\uXXXX format not supported"));
                break;
            default:
                throw JSONTokenizerError(string("JSONTokenizer::parseString invalid \\ char '") + c + "'");
                break;
            }
        }
        else
        {
            if(c == '"')
            {
                return Value(s);
            }
            s += c;
        }

    }

}

void JSONParser::parseKeyValue(map<Value,Value>& m)
{
    Value k = parseString();
    consume(':');
    Value v = parseValue();

    m[k] = v;
}

Value JSONParser::parseObject()
{


    consume("{");
    char c = peek();
    if(c == '}')
    {
        consume(c);
        return Value::makeMap();
    }

    map<Value,Value> m;

    for(;;) {

        parseKeyValue(m);

        char c = peek();
        if(c == '}')
        {
            consume(c);
            return Value::makeMap(m);
        }

        consume(',');

    }
}

Value JSONParser::parseArray()
{

    consume("[");
    char c = peek();
    if(c == ']')
    {
        consume(c);
        //cout << "JSONTokenizer::parseArray <== " << endl;;
        return Value::makeList();
    }

    vector<Value> l;
    for(;;) {

        l.push_back(parseValue());

        char c = peek();
        if(c == ']')
        {
            consume(c);
            //cout << "JSONTokenizer::parseArray <== " << endl;;
            return Value::makeList(l);
        }

        consume(',');

    }
}


Value JSONParser::parseValue()
{
    char c = peek();
    switch(c)
    {

    case 't': return parseTrue(); break;
    case 'f': return parseFalse(); break;
    case 'n': return parseNull(); break;
    case '{': return parseObject(); break;
    case '[': return parseArray(); break;
    case '\"': return parseString(); break;

    case '-': return parseNumber(); break;
    case '0': return parseNumber(); break;
    case '1': return parseNumber(); break;
    case '2': return parseNumber(); break;
    case '3': return parseNumber(); break;
    case '4': return parseNumber(); break;
    case '5': return parseNumber(); break;
    case '6': return parseNumber(); break;
    case '7': return parseNumber(); break;
    case '8': return parseNumber(); break;
    case '9': return parseNumber(); break;

    default:
        throw JSONTokenizerError(string("JSONTokenizer::parseValue unexpected char '") + c + "'");
        break;
    }
}

Value JSONParser::parse()
{
    Value v = parseValue();
    char c = peek();
    if(c != 0)
        throw JSONTokenizerError(string("JSONTokenizer::parse extra char '") + c + "'");
    return v;

}

//int main()
//{
//    JSONParser j(cin);
//    j.parse();
//    return 0;
//}
