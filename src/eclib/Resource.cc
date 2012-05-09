/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/Resource.h"
#include "eclib/Translator.h"

template<class T> void Resource<T>::setValue(const string& s)
{
    value_ = Translator<string,T>()(s);
}

template<class T> string Resource<T>::getValue() const
{
    return Translator<T,string>()(value_);
}
