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

void JSONParser::parseFalse()
{
    consume("false");
    return Value(false);
}

void JSONParser::parseNull()
{
    consume("null");
    return Value();
}

void JSONParser::parseNumber()
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

    cout << s << endl;

}

void JSONParser::parseString()
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
                return;
            }
            s += c;
        }

    }

}

void JSONParser::parseKeyValue()
{
    parseString();
    consume(':');
    parseValue();
}

void JSONParser::parseObject()
{
    consume("{");
    char c = peek();
    if(c == '}')
    {
        consume(c);
        return;
    }

    for(;;) {

        parseKeyValue();

        char c = peek();
        if(c == '}')
        {
            consume(c);
            return;
        }

        consume(',');

    }
}

void JSONParser::parseArray()
{
    //cout << "JSONTokenizer::parseArray ==> " << endl;;
    consume("[");
    char c = peek();
    if(c == ']')
    {
        consume(c);
        //cout << "JSONTokenizer::parseArray <== " << endl;;
        return;
    }

    for(;;) {

        parseValue();

        char c = peek();
        if(c == ']')
        {
            consume(c);
            //cout << "JSONTokenizer::parseArray <== " << endl;;
            return;
        }

        consume(',');

    }
}


void JSONParser::parseValue()
{
    char c = peek();
    switch(c)
    {

    case 't': parseTrue(); break;
    case 'f': parseFalse(); break;
    case 'n': parseNull(); break;
    case '{': parseObject(); break;
    case '[': parseArray(); break;
    case '\"': parseString(); break;

    case '-': parseNumber(); break;
    case '0': parseNumber(); break;
    case '1': parseNumber(); break;
    case '2': parseNumber(); break;
    case '3': parseNumber(); break;
    case '4': parseNumber(); break;
    case '5': parseNumber(); break;
    case '6': parseNumber(); break;
    case '7': parseNumber(); break;
    case '8': parseNumber(); break;
    case '9': parseNumber(); break;

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
