/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Exporter.h
// Baudouin Raoult - (c) ECMWF Oct 10

#ifndef Exporter_H
#define Exporter_H


#include "eclib/machine.h"

#include "eclib/NonCopyable.h"

class DataHandle;

class Exporter : private NonCopyable {

    class Datatype  {
        char type_;
        bool used_;
        double double_;
        long long signed_;
        unsigned long long unsigned_;

    public:
        Datatype();
        Datatype(double d);
        Datatype(long long d);
        Datatype(unsigned long long d);

        operator double();
        operator long long();
        operator unsigned long long();

        bool empty() const { return type_ == 0; }
        bool used() const { return used_; }

        void print(ostream& s) const;

        friend ostream& operator<<(ostream& s,const Datatype& p) { p.print(s); return s; }
    };

public:

// -- Exceptions
	// None

// -- Contructors

	Exporter(DataHandle&);

// -- Destructor

	~Exporter(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    void openForWrite();
    void close();

    DataHandle& handle() const { return handle_; }

    void startSchemas();
    void endSchemas();

    void startDatabases();
    void endDatabases();

    void startDatabase(const string&, unsigned long, unsigned long long);
    void endDatabase(const string&, unsigned long);
    
    void writeTag(char);
    char readTag();

    void writeSigned(long long);
    void writeUnsigned(unsigned long long);
    void writeString(const string&);
    void writeString(const char*);
    void writeDouble(double);

    void dataBase(unsigned long long, const string&);
    void typeInfo(unsigned long long, const string&);

    long long readSigned();
    unsigned long long readUnsigned();
    string readString();
    double readDouble();

    bool nextDatabase(string& name,unsigned long long& id, unsigned long long& count);

    size_t nextObject();
    void endObject();
    void nextSubObject();

    unsigned long long type() const     { return type_; }
    void type(unsigned long long  t)    { type_ = t; }

    unsigned long long location() const { return location_; }
    
    unsigned long long objectId() const { return objectId_; }


    void startObject(unsigned long long, unsigned long long, unsigned long long, size_t);
    void endObject(unsigned long long, unsigned long long, unsigned long long, size_t);

    void startSubObject();
    void endSubObject();

    unsigned long long getUnsignedMember(const string&);
    long long          getSignedMember(const string&);
    double             getDoubleMember(const string&);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods
	
	void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private: // members

    DataHandle& handle_;
    unsigned long long type_;
    unsigned long long location_;
    unsigned long long objectId_;

    unsigned long long objectCount_;
    unsigned long long subCount_;

    vector<string> stack_;
    map<string,Datatype> members_;

    bool inObject_;

// -- Methods
    long long _readSigned();
    unsigned long long _readUnsigned();
    string _readString();
    double _readDouble();

    string path() const;
	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend ostream& operator<<(ostream& s,const Exporter& p)
		{ p.print(s); return s; }

};

#endif
