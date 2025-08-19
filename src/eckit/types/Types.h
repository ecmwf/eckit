/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Types.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_Types_h
#define eckit_Types_h

#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "eckit/runtime/TaskID.h"  // to be removed

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef unsigned long Ordinal;  ///< for counting

typedef std::vector<Ordinal> OrdinalList;

typedef std::vector<std::string> StringList;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringDict;

//----------------------------------------------------------------------------------------------------------------------

template <typename S, typename T>
inline std::ostream& operator<<(std::ostream& s, const std::pair<S, T>& p) {
    s << "<" << p.first << "," << p.second << ">";
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class output_list {

    std::vector<T> v_;
    bool first_;
    std::ostream& s_;

    void flush();

public:

    void push_back(const T&);
    output_list(std::ostream&);
    ~output_list();
};

struct output_iterator {
    typedef std::output_iterator_tag iterator_category;
    typedef void value_type;
    typedef void difference_type;
    typedef void pointer;
    typedef void reference;
};

template <class T>
class output_list_iterator : public output_iterator {

    output_list<T>* list_;

public:

    output_list_iterator(output_list<T>* l) : list_(l) {}
    ~output_list_iterator() {}

    output_list_iterator<T>& operator=(const T& v) {
        list_->push_back(v);
        return *this;
    }
    output_list_iterator<T>& operator*() { return *this; }
    output_list_iterator<T>& operator++() { return *this; }
    output_list_iterator<T>& operator++(int) { return *this; }
};

//----------------------------------------------------------------------------------------------------------------------

class VectorPrintSimple {};
class VectorPrintContracted {};

template <typename T>
struct VectorPrintSelector {
    typedef VectorPrintContracted selector;
};

template <>
struct VectorPrintSelector<std::string> {
    typedef VectorPrintSimple selector;
};
template <>
struct VectorPrintSelector<double> {
    typedef VectorPrintSimple selector;
};
template <typename K, typename V>
struct VectorPrintSelector<std::pair<K, V> > {
    typedef VectorPrintSimple selector;
};


template <class T>
inline std::ostream& __print_list(std::ostream& s, const T& t, VectorPrintContracted) {
    output_list<typename T::value_type> l(s);
    output_list_iterator<typename T::value_type> os(&l);
    std::copy(t.begin(), t.end(), os);
    return s;
}

template <typename T>
inline std::ostream& __print_list(std::ostream& s, const std::vector<T>& t, VectorPrintSimple) {

    s << '[';
    for (Ordinal i = 0; i < t.size(); i++) {
        if (i != 0) {
            s << ',';
        }
        s << t[i];
    }
    s << ']';
    return s;
}

template <typename T, std::size_t N>
inline std::ostream& __print_list(std::ostream& s, const std::array<T, N>& t, VectorPrintSimple) {
    s << '[';
    for (Ordinal i = 0; i < t.size(); i++) {
        if (i != 0) {
            s << ',';
        }
        s << t[i];
    }
    s << ']';
    return s;
}

template <typename K, typename V, typename C>
inline std::ostream& __print_container(std::ostream& s, const std::map<K, V, C>& m) {
    const char* sep = "";
    s << "{";
    for (typename std::map<K, V>::const_iterator it = m.begin(); it != m.end(); ++it) {
        s << sep << it->first << "=" << it->second;
        sep = ",";
    }
    s << "}";
    return s;
}

template <typename T>
inline std::ostream& __print_container(std::ostream& s, const std::set<T>& m) {
    const char* sep = "";
    s << "{";
    for (typename std::set<T>::const_iterator it = m.begin(); it != m.end(); ++it) {
        s << sep << *it;
        sep = ",";
    }
    s << "}";
    return s;
}

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

namespace std {

// n.b. This overload needs to go either in the namespace std:: (which contains
//      ostream, vector), the global namespace, or the namespace containing T.
//      Otherwise it will not be found when doing lookups.
//
//  --> Probably best to put it in std::. It is acceptable to add "template
//      specializations for any standard library template" if the "declaration
//      depends on user-defined types".

template <class T>
inline std::ostream& operator<<(std::ostream& s, const std::vector<T>& v) {
    return eckit::__print_list(s, v, typename eckit::VectorPrintSelector<T>::selector());
}

template <class T, std::size_t N>
inline std::ostream& operator<<(std::ostream& s, const std::array<T, N>& v) {
    return eckit::__print_list(s, v, typename eckit::VectorPrintSelector<T>::selector());
}

template <typename K, typename V, typename C>
inline std::ostream& operator<<(std::ostream& s, const std::map<K, V, C>& m) {
    return eckit::__print_container(s, m);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& s, const std::set<T>& m) {
    return eckit::__print_container(s, m);
}
}  // namespace std

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;  // forward

/// Operators to send vectors in streams
template <class T>
Stream& operator<<(Stream&, const std::vector<T>&);
template <class T>
Stream& operator>>(Stream&, std::vector<T>&);

/// Operators to send sets in streams
template <class T>
Stream& operator<<(Stream&, const std::vector<T>&);
template <class T>
Stream& operator>>(Stream&, std::vector<T>&);

/// Operators to send maps in streams
/// Note: the value type V must have a constructor from Stream&
template <class K, class V>
Stream& operator<<(Stream&, const std::map<K, V>&);
template <class K, class V>
Stream& operator>>(Stream&, std::map<K, V>&);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/types/Types.cc"

#endif
