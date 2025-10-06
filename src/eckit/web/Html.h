/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_web_Html_H
#define eckit_web_Html_H

#include <map>
#include <string>

#include "eckit/web/HtmlObject.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Url;

class Html {
public:

    enum {
        None   = 0,
        Left   = 1,
        Right  = 2,
        Center = 4,
        Top    = 8,
        Bottom = 16
    };

    class Tag {
        virtual void print(std::ostream& s) const = 0;
        friend std::ostream& operator<<(std::ostream& s, const Tag&);
    };

    class RawTag : public Tag {
        std::string tag_;
        void print(std::ostream& s) const override { s << tag_; }

    protected:

        RawTag(const std::string& s) : tag_(s) {}
    };

    class Line : public RawTag {
    public:

        Line() : RawTag("<HR>") {}
    };

    // To be used with 'include'

    class Substitute : public HtmlObject {
        std::map<std::string, std::string, std::less<std::string> > map_;
        virtual void substitute(std::ostream&, const std::string&);

    public:

        Substitute();
        ~Substitute();
        std::string& operator[](const std::string&);
    };

    class Include {
    public:

        Include(const std::string&, HtmlObject* = 0);
        Include(const std::string&, HtmlObject&);

    private:

        Include(const Include&);
        Include& operator=(const Include&);

        HtmlObject* sub_;
        std::string name_;

        void print(std::ostream& s) const;

        friend std::ostream& operator<<(std::ostream& s, const Include& n) {
            n.print(s);
            return s;
        }
    };

    class Image : public Tag {
    public:

        Image(const std::string&);

        static std::string resource();

    private:

        Image(const Image&);
        Image& operator=(const Image&);

        std::string name_;
        void print(std::ostream& s) const override;
    };

    class Link : public Tag {
    public:

        Link(Url&);
        Link() {}
        Link(const std::string& url) : url_(addHex(url)) {}

    private:

        void print(std::ostream& s) const override;
        std::string url_;
    };

    class Class {
    public:

        Class(const std::string& str) : str_(str) {}

    private:

        std::string str_;
        void print(std::ostream& s) const;
        friend std::ostream& operator<<(std::ostream& s, const Class& n) {
            n.print(s);
            return s;
        }
    };

    //----------------------------------------------------------------------------------------------------------------------

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

    class BeginH1 : public RawTag {
    public:

        BeginH1() : RawTag("<H1>") {}
    };

    class EndH1 : public RawTag {
    public:

        EndH1() : RawTag("</H1>") {}
    };

    //----------------------------------------------------------------------------------------------------------------------

    class BeginForm : public Tag {
    public:

        BeginForm(const std::string& str = "") : str_(str) {}

    private:

        std::string str_;
        void print(std::ostream& s) const override;
    };

    class EndForm : public Tag {
        void print(std::ostream& s) const override;
    };

    class BeginTextArea {
    public:

        BeginTextArea(const std::string& name, int row, int col) : name_(name), row_(row), col_(col) {}

    private:

        std::string name_;
        int row_;
        int col_;
        void print(std::ostream& s) const;
        friend std::ostream& operator<<(std::ostream& s, const BeginTextArea& n) {
            n.print(s);
            return s;
        }
    };

    class EndTextArea {
        void print(std::ostream& s) const;
        friend std::ostream& operator<<(std::ostream& s, const EndTextArea& n) {
            n.print(s);
            return s;
        }
    };

    class TextField {
        std::string name_;
        std::string value_;
        std::string title_;
        void print(std::ostream& s) const;

    public:

        TextField(const std::string& name, const std::string& value = "", const std::string& title = "") :
            name_(name), value_(value), title_(title) {}
        friend std::ostream& operator<<(std::ostream& s, const TextField& n) {
            n.print(s);
            return s;
        }
    };

    class HiddenField : public Tag {
        std::string name_;
        std::string value_;
        void print(std::ostream& s) const override;

    public:

        HiddenField(const std::string& name, const std::string& value) : name_(name), value_(value) {}
    };


    class Button : public Tag {
        std::string type_;
        std::string title_;
        void print(std::ostream& s) const override;

    public:

        Button(const std::string& type, const std::string& title) : type_(type), title_(title) {}
    };

    class CheckBox : public Tag {
        std::string name_;
        std::string value_;
        bool on_;
        void print(std::ostream& s) const override;

    public:

        CheckBox(const std::string& name, const std::string& value, bool on) : name_(name), value_(value), on_(on) {}
    };

    class ResetButton : public Button {
    public:

        ResetButton(const std::string& title = "Reset") : Button("reset", title) {}
    };

    class SubmitButton : public Button {
    public:

        SubmitButton(const std::string& title = "Submit") : Button("submit", title) {}
    };

    class Hidden : public Tag {};

    static std::string addHex(const std::string&);
    static std::string removeHex(const std::string&);

    //----------------------------------------------------------------------------------------------------------------------

    // Table stuff

    class BeginTable : public Tag {
        bool border_;
        int padding_;
        int spacing_;
        int width_;
        void print(std::ostream& s) const override;

    public:

        BeginTable(bool border = true, int width = 0, int padding = 0, int spacing = 0) :
            border_(border), padding_(padding), spacing_(spacing), width_(width) {}
    };

    class EndTable : public RawTag {
    public:

        EndTable() : RawTag("</TABLE>") {}
    };


    class TableTag : public Tag {
        std::string tag_;
        int align_;
        int colspan_;
        int rowspan_;
        void print(std::ostream& s) const override;

    protected:

        TableTag(const std::string& tag, int align, int colspan, int rowspan) :
            tag_(tag), align_(align), colspan_(colspan), rowspan_(rowspan) {}
    };

    class BeginHeader : public TableTag {
    public:

        BeginHeader(int align = None, int colspan = 0, int rowspan = 0) : TableTag("TH", align, colspan, rowspan) {}
    };

    class EndHeader : public RawTag {
    public:

        EndHeader() : RawTag("</TH>") {}
    };

    class BeginRow : public TableTag {
    public:

        BeginRow(int align = None, int colspan = 0, int rowspan = 0) : TableTag("TR", align, colspan, rowspan) {}
    };

    class EndRow : public RawTag {
    public:

        EndRow() : RawTag("</TR>") {}
    };

    class BeginData : public TableTag {
    public:

        BeginData(int align = None, int colspan = 0, int rowspan = 0) : TableTag("TD", align, colspan, rowspan) {}
    };

    class EndData : public RawTag {
    public:

        EndData() : RawTag("</TD>") {}
    };

};  // class Html

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif
