/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Exporter.h
// Baudouin Raoult - (c) ECMWF Oct 10

#ifndef eckit_Exporter_h
#define eckit_Exporter_h


#include <map>
#include <vector>

#include "eckit/memory/NonCopyable.h"


namespace eckit {

//-----------------------------------------------------------------------------

class DataHandle;

class Exporter : private NonCopyable {

    class Datatype {
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

        void print(std::ostream& s) const;

        friend std::ostream& operator<<(std::ostream& s, const Datatype& p) {
            p.print(s);
            return s;
        }
    };

public:

    // -- Exceptions
    // None

    // -- Contructors

    Exporter(DataHandle&);

    // -- Destructor

    ~Exporter();

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

    void startDatabase(const std::string&, unsigned long, unsigned long long);
    void endDatabase(const std::string&, unsigned long);

    void writeTag(char);
    char readTag();

    void writeSigned(long long);
    void writeUnsigned(unsigned long long);
    void writeString(const std::string&);
    void writeString(const char*);
    void writeDouble(double);

    void dataBase(unsigned long long, const std::string&);
    void typeInfo(unsigned long long, const std::string&);

    long long readSigned();
    unsigned long long readUnsigned();
    std::string readString();
    double readDouble();

    bool nextDatabase(std::string& name, unsigned long long& id, unsigned long long& count);

    size_t nextObject();
    void endObject();
    void nextSubObject();

    unsigned long long type() const { return type_; }
    void type(unsigned long long t) { type_ = t; }

    unsigned long long location() const { return location_; }

    unsigned long long objectId() const { return objectId_; }


    void startObject(unsigned long long, unsigned long long, unsigned long long, size_t);
    void endObject(unsigned long long, unsigned long long, unsigned long long, size_t);

    void startSubObject();
    void endSubObject();

    unsigned long long getUnsignedMember(const std::string&);
    long long getSignedMember(const std::string&);
    double getDoubleMember(const std::string&);

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

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:  // members

    DataHandle& handle_;
    unsigned long long type_;
    unsigned long long location_;
    unsigned long long objectId_;

    unsigned long long objectCount_;
    unsigned long long subCount_;

    std::vector<std::string> stack_;
    std::map<std::string, Datatype> members_;

    bool inObject_;

    // -- Methods
    long long _readSigned();
    unsigned long long _readUnsigned();
    std::string _readString();
    double _readDouble();

    std::string path() const;


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Exporter& p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
