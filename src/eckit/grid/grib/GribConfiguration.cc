/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/grid/grib/GribConfiguration.h"

#include "eckit/value/Value.h"


namespace eckit::grid::grib {


static Value __root_dummy;


GribConfiguration::GribConfiguration(codes_handle* h) :
    Configuration(__root_dummy), codes_handle_(h, &codes_handle_delete) {
    ASSERT(codes_handle_);
}


bool GribConfiguration::has(const std::string& name) const {
    return 0 != codes_is_defined(codes_handle_.get(), name.c_str());
}


bool GribConfiguration::get(const std::string& name, std::string& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    char mesg[1024];
    if (auto length = sizeof(mesg);
        CODES_SUCCESS == codes_get_string(codes_handle_.get(), name.c_str(), mesg, &length)) {
        value = mesg;
        cache_.set(name, value);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, bool& value) const {
    if (long another = 0; get(name, another)) {
        value = another != 0;
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, int& value) const {
    if (long another = 0; get(name, another)) {
        value = static_cast<int>(another);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, long& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    if (CODES_SUCCESS == codes_get_long(codes_handle_.get(), name.c_str(), &value)) {
        cache_.set(name, value);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, long long& value) const {
    NOTIMP;
}


bool GribConfiguration::get(const std::string& name, std::size_t& value) const {
    if (long another = 0; get(name, another) && 0 <= another) {
        value = static_cast<std::size_t>(another);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, float& value) const {
    if (double another = 0; get(name, another)) {
        value = static_cast<float>(another);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, double& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    if (CODES_SUCCESS == codes_get_double(codes_handle_.get(), name.c_str(), &value)) {
        cache_.set(name, value);
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<int>& value) const {
    if (std::vector<long> another; get(name, another)) {
        value.resize(another.size());
        std::transform(another.begin(), another.end(), value.begin(), [](long v) {
            return static_cast<int>(v);
        });
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<long>& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    if (size_t size = 0; CODES_SUCCESS == codes_get_size(codes_handle_.get(), name.c_str(), &size)) {
        std::vector<long> another(size);
        if (CODES_SUCCESS
            == codes_get_long_array(codes_handle_.get(), name.c_str(), another.data(), &size)) {
            value.swap(another);
            cache_.set(name, value);
            return true;
        }
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<long long>& value) const {
    NOTIMP;
}


bool GribConfiguration::get(const std::string& name, std::vector<std::size_t>& value) const {
    if (std::vector<long> another; get(name, another)) {
        value.resize(another.size());
        std::transform(another.begin(), another.end(), value.begin(), [](long v) {
            ASSERT(0 <= v);
            return static_cast<std::size_t>(v);
        });
        return true;
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<float>& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    if (size_t size = 0; CODES_SUCCESS == codes_get_size(codes_handle_.get(), name.c_str(), &size)) {
        std::vector<float> another(size);
        if (CODES_SUCCESS
            == codes_get_float_array(codes_handle_.get(), name.c_str(), another.data(), &size)) {
            value.swap(another);
            cache_.set(name, value);
            return true;
        }
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<double>& value) const {
    if (cache_.get(name, value)) {
        return true;
    }

    if (size_t size = 0; CODES_SUCCESS == codes_get_size(codes_handle_.get(), name.c_str(), &size)) {
        std::vector<double> another(size);
        if (CODES_SUCCESS
            == codes_get_double_array(codes_handle_.get(), name.c_str(), another.data(), &size)) {
            value.swap(another);
            cache_.set(name, value);
            return true;
        }
    }

    return false;
}


bool GribConfiguration::get(const std::string& name, std::vector<std::string>& value) const {
    NOTIMP;
}


void GribConfiguration::print(std::ostream&) const {
    NOTIMP;
}


}  // namespace eckit::grid::grib
