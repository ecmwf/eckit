#pragma once

//-----------------------------------------------------------------------------

namespace eckit {

// Overload pattern for visiting std::variant using std::visit, see
// https://en.cppreference.com/w/cpp/utility/variant/visit

// The struct Overloaded can have arbitrary many base classes (Ts ...). It publicly inherits from each class and brings
// the call operator (Ts::operator...) of each base class into its scope. The base classes need an overloaded call
// operator (Ts::operator()).


// Overload pattern
template <typename... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

// Explicit deduction guide for the overload pattern above (not needed as of C++20)
template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

}  // namespace eckit


//-----------------------------------------------------------------------------
