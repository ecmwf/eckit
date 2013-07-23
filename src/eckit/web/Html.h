/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Html_H
#define Html_H

#include "eckit/eckit.h"

#include "eckit/web/HtmlObject.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Url;

class Html {
public:

	enum { None = 0, Left = 1, Right = 2, Center = 4, 
		Top = 8, Bottom = 16 };

    class Tag {
        virtual void print(ostream& s) const = 0;
        friend ostream& operator<<(ostream& s, const Tag&);
    };

    class RawTag : public Tag {
        string tag_;
        virtual void print(ostream& s) const { s << tag_; }
    protected:
        RawTag(const string& s) : tag_(s) {}
    };

    class Line : public RawTag {
    public:
        Line() : RawTag("<HR>") {}
    };

    // To be used with 'include'

    class Substitute : public HtmlObject {
        map<string,string,less<string> > map_;
        virtual void substitute(ostream&,const string&);
    public:
        Substitute();
        ~Substitute();
        string& operator[](const string&);
    };

    class Include {
    public:

        Include(const string&,HtmlObject* = 0);
        Include(const string&,HtmlObject&);

    private:

        Include(const Include&);
        Include& operator=(const Include&);

        HtmlObject*          sub_;
        string               name_;

        void print(ostream& s) const;

        friend ostream& operator<<(ostream& s, const Include& n)
        { n.print(s); return s; }
    };

    class Image : public Tag {
    public:
        Image(const string&);

        static string resource();

    private:
        Image(const Image&);
        Image& operator=(const Image&);

        string  name_;
        virtual void print(ostream& s) const;
    };

    class Link : public Tag {
    public:
        Link(Url&);
        Link() {}
        Link(const string& url) : url_(addHex(url)) {}
    private:
        virtual void print(ostream& s) const;
        string url_;
    };

    class Class {
    public:
        Class(const string& str) : str_(str) {}
    private:
        string str_;
        void print(ostream& s) const;
        friend ostream& operator<<(ostream& s, const Class& n)
        { n.print(s); return s; }
    };

    //-----------------------------------------------------------------------------

    class BeginFormatted : public RawTag {
    public:
        BeginFormatted() : RawTag("<PRE>") {}
    };

    class EndFormatted : public RawTag {
    public:
        EndFormatted() : RawTag("</PRE>") {}
    };
    class BeginFixed : public RawTag {
    public:
        BeginFixed() : RawTag("<TT>") {}
    };

    class EndFixed : public RawTag {
    public:
        EndFixed() : RawTag("</TT>") {}
    };

    class BeginBold : public RawTag {
    public:
        BeginBold() : RawTag("<B>") {}
    };

    class EndBold : public RawTag {
    public:
        EndBold() : RawTag("</B>") {}
    };

    //-----------------------------------------------------------------------------

    class BeginForm : public Tag {
    public:
        BeginForm(const string& str = "") : str_(str) {}
    private:
        string str_;
        virtual void print(ostream& s) const;
    };

    class EndForm : public Tag {
        virtual void print(ostream& s) const;
    };

    class BeginTextArea {
    public:
        BeginTextArea(const string& name,int row,int col):
            name_(name), row_(row),col_(col) {}
    private:
        string name_;
        int row_;
        int col_;
        void print(ostream& s) const;
        friend ostream& operator<<(ostream& s, const BeginTextArea& n)
        { n.print(s); return s; }
    };

    class EndTextArea {
        void print(ostream& s) const;
        friend ostream& operator<<(ostream& s, const EndTextArea& n)
        { n.print(s); return s; }
    };

    class TextField {
        string name_;
        string value_;
        string title_;
        void print(ostream& s) const;
    public:
        TextField(const string& name,const string& value = "",const string& title = ""):
            name_(name),value_(value),title_(title) {}
        friend ostream& operator<<(ostream& s, const TextField& n)
        { n.print(s); return s; }
    };

    class HiddenField : public Tag {
        string name_;
        string value_;
        virtual void print(ostream& s) const;
    public:
        HiddenField(const string& name,const string& value):
            name_(name),value_(value) {}
    };


    class Button : public Tag {
        string type_;
        string title_;
        virtual void print(ostream& s) const;
    public:
        Button(const string& type,const string& title):
            type_(type),title_(title) {}
    };

    class CheckBox : public Tag {
        string name_;
        string value_;
        bool   on_;
        virtual void print(ostream& s) const;
    public:
        CheckBox(const string& name,const string& value,bool on):
            name_(name), value_(value), on_(on) {}
    };

    class ResetButton : public Button {
    public:
        ResetButton(const string& title = "Reset"):
            Button("reset",title) {}
    };

    class SubmitButton : public Button {
    public:
        SubmitButton(const string& title = "Submit"):
            Button("submit",title) {}
    };

    class Hidden : public Tag {
    };

    static string addHex(const string&);
    static string removeHex(const string&);

    //-----------------------------------------------------------------------------

    // Table stuff

    class BeginTable : public Tag {
        bool border_;
        int  padding_;
        int  spacing_;
        int width_;
        virtual void print(ostream& s) const;
    public:
        BeginTable(bool border = true,int width = 0,
                   int padding = 0, int spacing = 0):
            border_(border), padding_(padding), spacing_(spacing), width_(width) {}
    };

    class EndTable : public RawTag {
    public:
        EndTable() : RawTag("</TABLE>") {}
    };


    class TableTag : public Tag {
        string tag_;
        int align_;
        int colspan_;
        int rowspan_;
        virtual void print(ostream& s) const;
    protected:
        TableTag(const string& tag,int align,int colspan, int rowspan):
            tag_(tag),align_(align),colspan_(colspan),rowspan_(rowspan) {}
    };

    class BeginHeader : public TableTag {
    public:
        BeginHeader(int align = None,int colspan = 0, int rowspan = 0):
            TableTag("TH",align,colspan,rowspan) {}
    };

    class EndHeader : public RawTag {
    public:
        EndHeader() : RawTag("</TH>") {}
    };

    class BeginRow : public TableTag {
    public:
        BeginRow(int align = None,int colspan = 0, int rowspan = 0):
            TableTag("TR",align,colspan,rowspan) {}
    };

    class EndRow : public RawTag {
    public:
        EndRow() : RawTag("</TR>") {}
    };

    class BeginData : public TableTag {
    public:
        BeginData(int align = None,int colspan = 0, int rowspan = 0):
            TableTag("TD",align,colspan,rowspan) {}
    };

    class EndData : public RawTag {
    public:
        EndData() : RawTag("</TD>") {}
    };

}; // class Html

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
