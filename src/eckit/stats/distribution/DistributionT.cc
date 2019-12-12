/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/distribution/DistributionT.h"

#include <iostream>
#include <random>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace stats {
namespace distribution {


namespace {

static DistributionBuilder<DistributionT<std::bernoulli_distribution>> __random1("bernoulli-distribution");
static DistributionBuilder<DistributionT<std::binomial_distribution<int>>> __random2("binomial-distribution");
static DistributionBuilder<DistributionT<std::cauchy_distribution<double>>> __random3("cauchy-distribution");
static DistributionBuilder<DistributionT<std::chi_squared_distribution<double>>> __random4("chi-squared-distribution");
static DistributionBuilder<DistributionT<std::discrete_distribution<int>>> __random5("discrete-distribution");
static DistributionBuilder<DistributionT<std::exponential_distribution<double>>> __random6("exponential-distribution");
static DistributionBuilder<DistributionT<std::extreme_value_distribution<double>>> __random7(
    "extreme-value-distribution");
static DistributionBuilder<DistributionT<std::fisher_f_distribution<double>>> __random8("fisher-f-distribution");
static DistributionBuilder<DistributionT<std::gamma_distribution<double>>> __random9("gamma-distribution");
static DistributionBuilder<DistributionT<std::geometric_distribution<int>>> __random10("geometric-distribution");
static DistributionBuilder<DistributionT<std::lognormal_distribution<double>>> __random11("lognormal-distribution");
static DistributionBuilder<DistributionT<std::negative_binomial_distribution<int>>> __random12(
    "negative-binomial-distribution");
static DistributionBuilder<DistributionT<std::normal_distribution<double>>> __random13("normal-distribution");
static DistributionBuilder<DistributionT<std::piecewise_constant_distribution<double>>> __random14(
    "piecewise-constant-distribution");
static DistributionBuilder<DistributionT<std::piecewise_linear_distribution<double>>> __random15(
    "piecewise-linear-distribution");
static DistributionBuilder<DistributionT<std::poisson_distribution<int>>> __random16("poisson-distribution");
static DistributionBuilder<DistributionT<std::student_t_distribution<double>>> __random17("student-t-distribution");
static DistributionBuilder<DistributionT<std::uniform_int_distribution<int>>> __random18("uniform-int-distribution");
static DistributionBuilder<DistributionT<std::uniform_real_distribution<double>>> __random19(
    "uniform-real-distribution");
static DistributionBuilder<DistributionT<std::weibull_distribution<double>>> __random20("weibull-distribution");

template <typename T>
T get_value(const param::MIRParametrisation& param, const std::string& key, T value) {
    param.get(key, value);
    return value;
}

template <typename T>
std::string put_vector(const std::vector<T>& v) {
    std::string s("[");
    auto sep = "";
    for (auto& value : v) {
        s += sep + std::to_string(value);
        sep = ",";
    }
    s += "]";
    return s;
}

}  // namespace


template <typename DISTRIBUTION>
DistributionT<DISTRIBUTION>::DistributionT(const param::MIRParametrisation& parametrisation) {
    auto p = param(parametrisation);
    distribution_.param(p);
}


template <typename DISTRIBUTION>
void DistributionT<DISTRIBUTION>::reset() {
    distribution_.reset();
}


template <typename DISTRIBUTION>
double DistributionT<DISTRIBUTION>::operator()() const {

    // random number engine (standard mersenne_twister_engine, with seed from random_device)
    static std::random_device randomDevice;
    static std::mt19937 gen(randomDevice());

    return distribution_(gen);
}


template <typename DISTRIBUTION>
void DistributionT<DISTRIBUTION>::print(std::ostream& out) const {
    auto p = distribution_.param();
    out << "Distribution[" << to_string(p) << ",min=" << distribution_.min() << ",max=" << distribution_.max() << "]";
}


template <typename DISTRIBUTION>
typename DISTRIBUTION::param_type DistributionT<DISTRIBUTION>::param(const param::MIRParametrisation&) const {
    NOTIMP;  // ensure specialisation
}


template <>
std::bernoulli_distribution::param_type DistributionT<std::bernoulli_distribution>::param(
    const param::MIRParametrisation& p) const {
    return std::bernoulli_distribution::param_type(get_value(p, "p", 0.5));
}


template <>
std::binomial_distribution<int>::param_type DistributionT<std::binomial_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    return std::binomial_distribution<int>::param_type(get_value(p, "t", 1), get_value(p, "p", 0.5));
}


template <>
std::cauchy_distribution<double>::param_type DistributionT<std::cauchy_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::cauchy_distribution<double>::param_type(get_value(p, "a", 0.), get_value(p, "b", 1.));
}


template <>
std::chi_squared_distribution<double>::param_type DistributionT<std::chi_squared_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::chi_squared_distribution<double>::param_type(get_value(p, "n", 1.));
}


template <>
std::discrete_distribution<int>::param_type DistributionT<std::discrete_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    std::vector<double> d;
    p.get("probabilities", d);
    ASSERT(!d.empty());
    return std::discrete_distribution<int>::param_type({d.begin(), d.end()});
}


template <>
std::exponential_distribution<double>::param_type DistributionT<std::exponential_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::exponential_distribution<double>::param_type(get_value(p, "lambda", 1.));
}


template <>
std::extreme_value_distribution<double>::param_type DistributionT<std::extreme_value_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::extreme_value_distribution<double>::param_type(get_value(p, "a", 0.), get_value(p, "b", 1.));
}


template <>
std::fisher_f_distribution<double>::param_type DistributionT<std::fisher_f_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::fisher_f_distribution<double>::param_type(get_value(p, "m", 1.), get_value(p, "n", 1.));
}
std::string name_;


template <>
std::gamma_distribution<double>::param_type DistributionT<std::gamma_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::gamma_distribution<double>::param_type(get_value(p, "alpha", 1.), get_value(p, "beta", 1.));
}


template <>
std::geometric_distribution<int>::param_type DistributionT<std::geometric_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    return std::geometric_distribution<int>::param_type(get_value(p, "p", 0.5));
}


template <>
std::lognormal_distribution<double>::param_type DistributionT<std::lognormal_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::lognormal_distribution<double>::param_type(get_value(p, "m", 0.), get_value(p, "s", 1.));
}


template <>
std::negative_binomial_distribution<int>::param_type DistributionT<std::negative_binomial_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    return std::negative_binomial_distribution<int>::param_type(get_value(p, "k", 1), get_value(p, "p", 0.5));
}


template <>
std::normal_distribution<double>::param_type DistributionT<std::normal_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::normal_distribution<double>::param_type(get_value(p, "mean", 0.), get_value(p, "stddev", 1.));
}


template <>
std::piecewise_constant_distribution<double>::param_type
DistributionT<std::piecewise_constant_distribution<double>>::param(const param::MIRParametrisation& p) const {
    std::vector<double> i;
    std::vector<double> w;
    p.get("intervals", i);
    p.get("densities", w);
    ASSERT(!w.empty());
    ASSERT(w.size() + 1 == i.size());
    return {i.begin(), i.end(), w.begin()};
}


template <>
std::piecewise_linear_distribution<double>::param_type DistributionT<std::piecewise_linear_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    std::vector<double> i;
    std::vector<double> w;
    p.get("intervals", i);
    p.get("densities", w);
    ASSERT(!w.empty());
    ASSERT(w.size() == i.size());
    return {i.begin(), i.end(), w.begin()};
}


template <>
std::poisson_distribution<int>::param_type DistributionT<std::poisson_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    return std::poisson_distribution<int>::param_type(get_value(p, "mean", 1.));
}


template <>
std::student_t_distribution<double>::param_type DistributionT<std::student_t_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::student_t_distribution<double>::param_type(get_value(p, "n", 1.));
}


template <>
std::uniform_int_distribution<int>::param_type DistributionT<std::uniform_int_distribution<int>>::param(
    const param::MIRParametrisation& p) const {
    return std::uniform_int_distribution<int>::param_type(get_value(p, "a", 0), get_value(p, "b", 1));
}


template <>
std::uniform_real_distribution<double>::param_type DistributionT<std::uniform_real_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::uniform_real_distribution<double>::param_type(get_value(p, "a", 0.), get_value(p, "b", 1.));
}


template <>
std::weibull_distribution<double>::param_type DistributionT<std::weibull_distribution<double>>::param(
    const param::MIRParametrisation& p) const {
    return std::weibull_distribution<double>::param_type(get_value(p, "a", 1.), get_value(p, "b", 1.));
}


template <typename DISTRIBUTION>
std::string DistributionT<DISTRIBUTION>::to_string(typename DISTRIBUTION::param_type&) const {
    NOTIMP;  // ensure specialisation
}


template <>
std::string DistributionT<std::bernoulli_distribution>::to_string(
    typename std::bernoulli_distribution::param_type& param) const {
    return "bernoulli-distribution{p:" + std::to_string(param.p()) + "}";
}


template <>
std::string DistributionT<std::binomial_distribution<int>>::to_string(
    typename std::binomial_distribution<int>::param_type& param) const {
    return "binomial-distribution{t:" + std::to_string(param.t()) + ",p:" + std::to_string(param.p()) + "}";
}


template <>
std::string DistributionT<std::cauchy_distribution<double>>::to_string(
    typename std::cauchy_distribution<double>::param_type& param) const {
    return "cauchy-distribution{a:" + std::to_string(param.a()) + ",b:" + std::to_string(param.b()) + "}";
}


template <>
std::string DistributionT<std::chi_squared_distribution<double>>::to_string(
    typename std::chi_squared_distribution<double>::param_type& param) const {
    return "chi-squared-distribution{n:" + std::to_string(param.n()) + "}";
}


template <>
std::string DistributionT<std::discrete_distribution<int>>::to_string(
    typename std::discrete_distribution<int>::param_type& param) const {
    return "discrete-distribution{probabilities:" + put_vector(param.probabilities()) + "}";
}


template <>
std::string DistributionT<std::exponential_distribution<double>>::to_string(
    typename std::exponential_distribution<double>::param_type& param) const {
    return "exponential-distribution{lambda:" + std::to_string(param.lambda()) + "}";
}


template <>
std::string DistributionT<std::extreme_value_distribution<double>>::to_string(
    typename std::extreme_value_distribution<double>::param_type& param) const {
    return "extreme-value-distribution{a:" + std::to_string(param.a()) + ",b:" + std::to_string(param.b()) + "}";
}


template <>
std::string DistributionT<std::fisher_f_distribution<double>>::to_string(
    typename std::fisher_f_distribution<double>::param_type& param) const {
    return "fisher-f-distribution{m:" + std::to_string(param.m()) + "," + ",n:" + std::to_string(param.n()) + "}";
}


template <>
std::string DistributionT<std::gamma_distribution<double>>::to_string(
    typename std::gamma_distribution<double>::param_type& param) const {
    return "gamma-distribution{alpha:" + std::to_string(param.alpha()) + "," + ",beta:" + std::to_string(param.beta()) +
           "}";
}


template <>
std::string DistributionT<std::geometric_distribution<int>>::to_string(
    typename std::geometric_distribution<int>::param_type& param) const {
    return "geometric-distribution{p:" + std::to_string(param.p()) + "}";
}


template <>
std::string DistributionT<std::lognormal_distribution<double>>::to_string(
    typename std::lognormal_distribution<double>::param_type& param) const {
    return "lognormal-distribution{m:" + std::to_string(param.m()) + "," + ",s:" + std::to_string(param.s()) + "}";
}


template <>
std::string DistributionT<std::negative_binomial_distribution<int>>::to_string(
    typename std::negative_binomial_distribution<int>::param_type& param) const {
    return "negative-binomial-distribution{k:" + std::to_string(param.k()) + "," + ",p:" + std::to_string(param.p()) +
           "}";
}


template <>
std::string DistributionT<std::normal_distribution<double>>::to_string(
    typename std::normal_distribution<double>::param_type& param) const {
    return "normal-distribution{mean:" + std::to_string(param.mean()) + "," +
           ",stddev:" + std::to_string(param.stddev()) + "}";
}


template <>
std::string DistributionT<std::piecewise_constant_distribution<double>>::to_string(
    typename std::piecewise_constant_distribution<double>::param_type& param) const {
    return "piecewise-constant-distribution{densities:" + put_vector(param.densities()) + "," +
           ",intervals:" + put_vector(param.intervals()) + "}";
}


template <>
std::string DistributionT<std::piecewise_linear_distribution<double>>::to_string(
    typename std::piecewise_linear_distribution<double>::param_type& param) const {
    return "piecewise-linear-distribution{densities:" + put_vector(param.densities()) + "," +
           ",intervals:" + put_vector(param.intervals()) + "}";
}


template <>
std::string DistributionT<std::poisson_distribution<int>>::to_string(
    typename std::poisson_distribution<int>::param_type& param) const {
    return "poisson-distribution{mean:" + std::to_string(param.mean()) + "}";
}


template <>
std::string DistributionT<std::student_t_distribution<double>>::to_string(
    typename std::student_t_distribution<double>::param_type& param) const {
    return "student-t-distribution{n:" + std::to_string(param.n()) + "}";
}


template <>
std::string DistributionT<std::uniform_int_distribution<int>>::to_string(
    typename std::uniform_int_distribution<int>::param_type& param) const {
    return "uniform-int-distribution{a:" + std::to_string(param.a()) + ",b:" + std::to_string(param.b()) + "}";
}


template <>
std::string DistributionT<std::uniform_real_distribution<double>>::to_string(
    typename std::uniform_real_distribution<double>::param_type& param) const {
    return "uniform-real-distribution{a:" + std::to_string(param.a()) + ",b:" + std::to_string(param.b()) + "}";
}


template <>
std::string DistributionT<std::weibull_distribution<double>>::to_string(
    typename std::weibull_distribution<double>::param_type& param) const {
    return "weibull-distribution{a:" + std::to_string(param.a()) + ",b:" + std::to_string(param.b()) + "}";
}


}  // namespace distribution
}  // namespace stats
}  // namespace mir
