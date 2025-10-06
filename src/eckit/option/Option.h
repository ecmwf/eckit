/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#ifndef Option_H
#define Option_H

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit::option {

class Option : private eckit::NonCopyable {
public:

    using args_t = std::vector<std::string>;

public:  // methods

    Option(const std::string& name, const std::string& description);
    virtual ~Option() = default;

    [[nodiscard]] const std::string& name() const { return name_; };
    [[nodiscard]] const std::string& description() const { return description_; }

    [[nodiscard]] virtual bool active() const { return true; };

    /**
     * Set the value of the option into `parameter`, taking as many values as necessary from the range `[begin, end)`.
     *
     * - `values` indicates the number of items in `[begin, end)` that were provided as an option value.
     *    This parameter is expected to be either 0 (for --flag options) or 1 (for traditional --option=<value>)
     *
     * Return: number of items consumed from the range `[begin, end)`.
     */
    virtual size_t set(Configured& param, size_t values, args_t::const_iterator begin,
                       args_t::const_iterator end) const = 0;

    virtual void copy(const Configuration& from, Configured& to) const = 0;

    virtual void setDefault(Configured&) const = 0;

    template <typename T>
    static void copy(const std::string& name, const Configuration& from, Configured& to) {
        // This is so generic that could probably be provided by Configuration or Configured
        T value;
        if (from.get(name, value)) {
            to.set(name, value);
        }
    }

protected:  // members

    std::string name_;
    std::string description_;

    virtual void print(std::ostream&) const = 0;

private:

    friend std::ostream& operator<<(std::ostream& s, const Option& p);
};

template <class T>
class BaseOption : public Option {
public:

    BaseOption(const std::string& name, const std::string& description) :
        Option(name, description), default_value_{std::nullopt} {};
    BaseOption(const std::string& name, const std::string& description, const T& default_value) :
        Option(name, description), default_value_{std::make_optional(default_value)} {};
    BaseOption(const std::string& name, const std::string& description, std::optional<T> default_value) :
        Option(name, description), default_value_{std::move(default_value)} {};
    ~BaseOption() override = default;

    [[deprecated("Specify the default value(s) when calling the ctor")]]
    Option* defaultValue(const std::string& value) {
        T translated   = translate(value);
        default_value_ = std::make_optional(translated);
        return this;
    }

    void setDefault(Configured& parametrisation) const final {
        if (default_value_) {
            set_value(default_value_.value(), parametrisation);
        }
    }

protected:

    virtual void set_value(const T& value, Configured& parametrisation) const = 0;

    // Performs the conversion from 'string' value to actual 'type' value
    virtual T translate(const std::string& value) const = 0;

private:

    std::optional<T> default_value_;
};


}  // namespace eckit::option

#endif
