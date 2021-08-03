/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/web/Html.h"
#include "eckit/web/HtmlObject.h"
#include "eckit/web/HtmlResource.h"
#include "eckit/web/HttpStream.h"
#include "eckit/web/Url.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& s, const Html::Tag& tag) {
    s << HttpStream::dontEncode;
    tag.print(s);
    s << HttpStream::doEncode;
    return s;
}

std::string Html::addHex(const std::string& s) {
    std::string t;
    int index  = 0;
    int length = s.length();

    while (index < length) {
        char c  = s[index];
        bool ok = false;

        switch (c) {
            case '+':
            case '&':
            case '/':
            case '=':
            case '?':
                ok = true;
                break;

            default:
                ok = isalnum(c);
                break;
        }

        if (ok)
            t += c;
        else {
            t += '%';

            unsigned int h = ((unsigned char)c) / 16;
            unsigned int l = ((unsigned char)c) % 16;

            if (h >= 10)
                c = h - 10 + 'A';
            else
                c = h + '0';
            t += c;

            if (l >= 10)
                c = l - 10 + 'A';
            else
                c = l + '0';
            t += c;
        }
        index++;
    }

    return t;
}

std::string Html::removeHex(const std::string& s) {
    std::string t;
    int index  = 0;
    int length = s.length();

    while (index < length) {
        if (s[index] == '+') {
            t += ' ';
        }
        else if (s[index] == '%') {
            char h = s[index + 1];
            char l = s[index + 2];
            index += 2;

            unsigned int a = (h >= 'A') ? (h - 'A' + 10) : (h - '0');
            unsigned int b = (l >= 'A') ? (l - 'A' + 10) : (l - '0');

            t += char(a * 16 + b);
        }
        else
            t += s[index];

        index++;
    }
    return t;
}

//----------------------------------------------------------------------------------------------------------------------


Html::Include::Include(const std::string& name, HtmlObject* sub) :
    sub_(sub), name_(name) {}

Html::Include::Include(const std::string& name, HtmlObject& sub) :
    sub_(&sub), name_(name) {}

//----------------------------------------------------------------------------------------------------------------------


void Html::Include::print(std::ostream& s) const {
    eckit::PathName path = eckit::Resource<PathName>("htmlPath", "~/html");

    path = path + '/' + name_;

    std::ifstream in(path.localPath());

    if (!in) {
        s << path << ": " << Log::syserr << std::endl;
        return;
    }


    char c;
    std::string p;
    bool word = false;


    s << HttpStream::dontEncode;

    while (in.get(c)) {
        if (c == '%') {
            if (word) {
                if (sub_)
                    sub_->substitute(s, p);
                else
                    s << '%' << p << '%';
                p    = "";
                word = false;
            }
            else
                word = true;
            in.get(c);
        }

        if (word)
            p += c;
        else
            s << c;
    }

    s << HttpStream::doEncode;
}

//----------------------------------------------------------------------------------------------------------------------


Html::Image::Image(const std::string& name) :
    name_(name) {}

void Html::Image::print(std::ostream& s) const {
    s << "<IMG SRC=\"" << resource() + '/' + name_ << "\">";
}

std::string Html::Image::resource() {
    return "/image";
}


//----------------------------------------------------------------------------------------------------------------------


Html::Link::Link(Url& url) :
    url_(addHex(url.str())) {}

void Html::Link::print(std::ostream& s) const {
    if (url_.length())
        s << "<A HREF=\"" << url_ << "\">";
    else
        s << "</A>";
}

//----------------------------------------------------------------------------------------------------------------------


void Html::Substitute::substitute(std::ostream& s, const std::string& p) {
    std::map<std::string, std::string, std::less<std::string> >::iterator i = map_.find(p);
    if (i == map_.end())
        s << '%' << p << '%';
    else
        s << HttpStream::doEncode << (*i).second << HttpStream::dontEncode;
}

Html::Substitute::Substitute() {}

Html::Substitute::~Substitute() {}


std::string& Html::Substitute::operator[](const std::string& p) {
    return map_[p];
}


void Html::Class::print(std::ostream& s) const {
    std::string p;
    long len         = str_.length();
    std::string base = "http://wwwec.ecmwf.int/dhs/classfinder?file=";

    for (int i = 0; i < len; i++) {
        char c = str_[i];
        if (isalnum(c) || c == '_')
            p += c;
        else if (p.length()) {
            s << Link(base + p) << p << Link();
            s << c;
            p = "";
        }
        else
            s << c;
    }
    if (p.length())
        s << Link(base + p) << p << Link();
}

void Html::BeginForm::print(std::ostream& s) const {
    s << "<FORM METHOD=\"POST\"";

    if (str_.length())
        s << " ACTION=\"" << str_ << "\"";

    s << ">";
}

void Html::EndForm::print(std::ostream& s) const {
    s << "</FORM>";
}

void Html::TextField::print(std::ostream& s) const {
    s << title_ << HttpStream::dontEncode;
    s << "<INPUT NAME=\"" << name_ << "\" VALUE=\"" << value_ << "\">";
    s << HttpStream::doEncode;
}

void Html::HiddenField::print(std::ostream& s) const {
    s << "<INPUT TYPE=\"hidden\" NAME=\"" << name_ << "\" VALUE=\"" << value_ << "\">";
}

void Html::CheckBox::print(std::ostream& s) const {
    s << "<INPUT TYPE=\"checkbox\" ";
    if (on_)
        s << "checked ";
    s << "NAME=\"" << name_ << "\" VALUE=\"" << value_ << "\">";
}

void Html::Button::print(std::ostream& s) const {
    s << "<INPUT TYPE=\"" << type_ << "\" VALUE=\"" << title_ << "\">";
}

void Html::BeginTextArea::print(std::ostream& s) const {
    s << HttpStream::dontEncode;
    s << "<TEXTAREA NAME=\"" << name_ << "\" ROWS=" << row_ << " COLS=" << col_ << ">";
}

void Html::EndTextArea::print(std::ostream& s) const {
    s << "</TEXTAREA>" << std::endl
      << HttpStream::doEncode;
}

//----------------------------------------------------------------------------------------------------------------------

class ImageProvider : public HtmlResource {
public:
    ImageProvider() :
        HtmlResource(Html::Image::resource()) {}
    void GET(std::ostream&, Url&);
};

static ImageProvider imageProvider;

void ImageProvider::GET(std::ostream& out, Url& url) {
    eckit::PathName path = eckit::Resource<PathName>("imagePath", "~/html/image");

    for (int i = 1; i < url.size(); i++)
        path = path + "/" + url[i];

    std::ifstream in(path.localPath());
    if (!in) {
        url.status(HttpError::NOT_FOUND);  // Not Found
        out << path << ": " << Log::syserr << std::endl;
    }
    else {
        (url.headerOut()).type("image/gif");

        out << HttpStream::dontEncode;
        char c;
        while (in.get(c))
            out << c;
        out << HttpStream::doEncode;
    }
}

//----------------------------------------------------------------------------------------------------------------------

class HtmlProvider : public HtmlResource {
public:
    HtmlProvider() :
        HtmlResource("/html") {}
    virtual ~HtmlProvider() {}
    void GET(std::ostream&, Url&);
};

static HtmlProvider htmlProvider;

void HtmlProvider::GET(std::ostream& s, Url& url) {
    std::string path;

    for (int i = 1; i < url.size(); i++)
        path += "/" + url[i];

    Html::Substitute empty;
    Html::Include include(path, empty);
    s << include;
}


//----------------------------------------------------------------------------------------------------------------------

void Html::BeginTable::print(std::ostream& s) const {
    s << "<TABLE";
    if (border_)
        s << " BORDER";
    if (padding_)
        s << " CELLPADDING=" << padding_;
    if (spacing_)
        s << " CELLSPACING=" << spacing_;
    if (width_)
        s << " WIDTH=" << '"' << width_ << '%' << '"';
    s << ">";
}

void Html::TableTag::print(std::ostream& s) const {
    s << '<' << tag_;

    if (align_) {

        if ((align_ & Center))
            s << " ALIGN=center";
        if ((align_ & Left))
            s << " ALIGN=left";
        if ((align_ & Right))
            s << " ALIGN=right";
        if ((align_ & Top))
            s << " VALIGN=top";
        if ((align_ & Bottom))
            s << " VALIGN=bottom";
    }

    if (colspan_)
        s << " COLSPAN=" << colspan_;
    if (rowspan_)
        s << " ROWSPAN=" << rowspan_;

    s << '>';
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
