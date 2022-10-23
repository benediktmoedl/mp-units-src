// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <units/bits/algorithm.h>
#include <units/bits/expression_template.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/text_tools.h>
#include <units/bits/external/type_name.h>
#include <units/bits/external/type_traits.h>
#include <units/magnitude.h>
#include <units/symbol_text.h>
#include <iterator>
#include <string>

// #include <units/bits/derived_symbol_text.h>

// IWYU pragma: begin_exports
// #include <units/bits/absolute_magnitude.h>
// #include <units/ratio.h>
// IWYU pragma: end_exports

namespace units {

namespace detail {

template<typename T>
inline constexpr bool is_unit = false;

}  // namespace detail

/**
 * @brief A concept matching all unit types in the library
 *
 * Satisfied by all unit types provided by the library.
 */
template<typename T>
concept Unit = detail::is_unit<T>;


/**
 * @brief Unit being a scaled version of another unit
 *
 * @tparam M magnitude describing the scale factor
 * @tparam U reference unit being scaled
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the unit arithmetic equation provided by the user.
 */
template<Magnitude auto M, Unit U>
struct scaled_unit {
  static constexpr UNITS_MSVC_WORKAROUND(Magnitude) auto mag = M;
  static constexpr U reference_unit{};
};

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_scaled_unit = false;

template<auto M, Unit U>
inline constexpr bool is_specialization_of_scaled_unit<scaled_unit<M, U>> = true;

}  // namespace detail

/**
 * @brief A named unit
 *
 * Defines a unit with a special name.
 * Most of the named units may be composed with a prefix to create a `prefixed_unit`.
 *
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct second : named_unit<"s"> {} second;
 * inline constexpr struct metre : named_unit<"m"> {} metre;
 * inline constexpr struct hertz : named_unit<"Hz", 1 / second> {} hertz;
 * inline constexpr struct newton : named_unit<"N", kilogram * metre / square<second>> {} newton;
 * inline constexpr struct degree_Celsius : named_unit<basic_symbol_text{"\u00B0C", "`C"}, kelvin> {} degree_Celsius;
 * inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the unit types in the source code. All operations
 *       are done on the objects. Contrarily, the unit types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol a short text representation of the unit
 */
template<basic_symbol_text Symbol, auto...>
struct named_unit;

/**
 * @brief Specialization for base unit
 *
 * Defines a base unit in the system of units (i.e. `metre`).
 * or a name assigned to another scaled or derived unit (i.e. `hour`, `joule`).
 * Most of the named units may be composed with a prefix to create a `prefixed_unit`.
 *
 * @tparam Symbol a short text representation of the unit
 */
template<basic_symbol_text Symbol>
  requires(!Symbol.empty())
struct named_unit<Symbol> {
  static constexpr auto symbol = Symbol;  ///< Unique base unit identifier
};

/**
 * @brief Specialization for a unit with special name
 *
 * Allows assigning a special name to another scaled or derived unit (i.e. `hour`, `joule`).
 *
 * @tparam Symbol a short text representation of the unit
 * @tparam Unit a unit for which we provide a special name
 */
template<basic_symbol_text Symbol, Unit auto U>
  requires(!Symbol.empty())
struct named_unit<Symbol, U> : std::remove_const_t<decltype(U)> {
  static constexpr auto symbol = Symbol;  ///< Unique unit identifier
};

namespace detail {

template<basic_symbol_text Symbol, auto... Args>
void to_base_specialization_of_named_unit(const volatile named_unit<Symbol, Args...>*);

template<typename T>
inline constexpr bool is_specialization_of_named_unit = false;

template<basic_symbol_text Symbol, auto... Args>
inline constexpr bool is_specialization_of_named_unit<named_unit<Symbol, Args...>> = true;

}  // namespace detail

/**
 * @brief A concept matching all units with special names
 *
 * Satisfied by all unit types derived from the specialization of `named_unit`.
 */
template<typename T>
concept NamedUnit = Unit<T> && requires(T* t) { detail::to_base_specialization_of_named_unit(t); } &&
                    (!detail::is_specialization_of_named_unit<T>);

/**
 * @brief Prevents assignment of a prefix to specific units
 *
 * By default all named units allow assigning a prefix for them. There are some notable exceptions like
 * `hour` or `degree_Celsius`. For those a partial specialization with the value `false` should be
 * provided.
 */
template<NamedUnit auto V>
inline constexpr bool unit_can_be_prefixed = true;


/**
 * @brief A concept to be used to define prefixes for a unit
 */
template<typename T>
concept PrefixableUnit = NamedUnit<T> && unit_can_be_prefixed<T{}>;


/**
 * @brief A prefixed unit
 *
 * Defines a new unit that is a scaled version of another unit with the scaling
 * factor specified by a predefined prefix.
 *
 * For example:
 *
 * @code{.cpp}
 * template<PrefixableUnit auto U>
 * struct kilo_ : prefixed_unit<"k", mag_power<10, 3>, U> {};
 *
 * template<PrefixableUnit auto U>
 * inline constexpr kilo_<U> kilo;
 *
 * inline constexpr struct kilogram : decltype(si::kilo<gram>) {} kilogram;
 * @endcode
 *
 * @tparam Symbol a prefix text to prepend to a unit symbol
 * @tparam M scaling factor of the prefix
 * @tparam U a named unit to be prefixed
 */
template<basic_symbol_text Symbol, Magnitude auto M, PrefixableUnit auto U>
  requires(!Symbol.empty())
struct prefixed_unit : std::remove_const_t<decltype(M * U)> {
  static constexpr auto symbol = Symbol + U.symbol;
};

namespace detail {

template<typename T>
inline constexpr bool is_power_of_unit =
  requires { requires is_specialization_of_power<T> && Unit<typename T::factor>; };

template<typename T>
inline constexpr bool is_per_of_units = false;

template<typename... Ts>
inline constexpr bool is_per_of_units<per<Ts...>> = (... && (Unit<Ts> || is_power_of_unit<Ts>));

}  // namespace detail

template<typename T>
concept DerivedUnitSpec = Unit<T> || detail::is_power_of_unit<T> || detail::is_per_of_units<T>;

/**
 * @brief Measurement unit for a derived quantity
 *
 * Derived units are defined as products of powers of the base units.
 *
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
 * more digestable for the user. The positive exponents are ordered first and all negative exponents are put as a list
 * into the `per<...>` class template. If a power of exponent is different than `1` the unit type is enclosed in
 * `power<Dim, Num, Den>` class template. Otherwise, it is just put directly in the list without any wrapper. There
 * is also one special case. In case all of the exponents are negative then the `one` being a coherent unit of
 * a dimensionless quantity is put in the front to increase the readability.
 *
 * For example:
 *
 * @code{.cpp}
 * static_assert(is_of_type<1 / second, derived_unit<one, per<second>>>);
 * static_assert(is_of_type<1 / (1 / second), second>);
 * static_assert(is_of_type<one * second, second>);
 * static_assert(is_of_type<metre * metre, derived_unit<power<metre, 2>>>);
 * static_assert(is_of_type<metre * second, derived_unit<metre, second>>);
 * static_assert(is_of_type<metre / second, derived_unit<metre, per<second>>>);
 * static_assert(is_of_type<metre / square<second>, derived_unit<metre, per<power<second, 2>>>>);
 * static_assert(is_of_type<watt / joule, derived_unit<watt, per<joule>>>);
 * @endcode
 *
 * Every unit in the library has its internal canonical representation being the list of exponents of named base units
 * (with the exception of `kilogram` which is represented as `gram` here) and a scaling ratio represented with a
 * magnitude.
 *
 * Two units are deemed convertible if their canonical version has units of the same type.
 * Two units are equivalent when they are convertible and their canonical versions have the same scaling ratios.
 *
 * The above means that:
 * - `1/s` and `Hz` are both convertible and equal
 * - `m` and `km` are convertible but not equal
 * - `m` and `m²` ane not convertible and not equal
 *
 * @note This also means that units like `hertz` and `becquerel` are also considered convertible and equal.
 *
 * @tparam Us a parameter pack consisting tokens allowed in the unit specification
 *            (units, `power<U, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the unit arithmetic equation provided by the user.
 */
template<DerivedUnitSpec... Us>
struct derived_unit : detail::expr_fractions<derived_unit<>, Us...> {};

/**
 * @brief Unit one
 *
 * Unit of a dimensionless quantity.
 */
inline constexpr struct one : derived_unit<> {
} one;


namespace detail {

template<auto M, typename U>
void is_unit_impl(const volatile scaled_unit<M, U>*);

template<basic_symbol_text Symbol, auto... Args>
void is_unit_impl(const volatile named_unit<Symbol, Args...>*);

template<typename... Us>
void is_unit_impl(const volatile derived_unit<Us...>*);

template<typename T>
  requires requires(T* t) { is_unit_impl(t); }
inline constexpr bool is_unit<T> = true;

/**
 * @brief A canonical representation of a unit
 *
 * A canonical representation of a unit consists of a `reference_unit` and its scaling
 * factor represented by the magnitude `mag`.
 *
 * `reference_unit` is a unit (possibly derived one) that consists only named base units.
 * All of the intermediate derived units are extracted, prefixes and magnitudes of scaled
 * units are stripped from them and accounted in the `mag`.
 *
 * All units having the same canonical unit are deemed equal.
 * All units having the same `reference_unit` are convertible (their `mag` may differ
 * and is the subject of conversion).
 *
 * @tparam U a unit to use as a `reference_unit`
 * @tparam M a Magnitude representing an absolute scaling factor of this unit
 */
template<Magnitude M, Unit U>
struct canonical_unit {
  M mag;
  U reference_unit;
};

template<Unit T, basic_symbol_text Symbol>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol>&);

template<Unit T, basic_symbol_text Symbol, Unit auto U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U>&);

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&);

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit<Us...>&);

template<Unit T, auto M, typename U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const scaled_unit<M, U>&)
{
  auto base = get_canonical_unit_impl(U{}, U{});
  return canonical_unit{M * base.mag, base.reference_unit};
}

template<Unit T, basic_symbol_text Symbol>
[[nodiscard]] consteval auto get_canonical_unit_impl(T t, const named_unit<Symbol>&)
{
  return canonical_unit{mag<1>, t};
}

template<Unit T, basic_symbol_text Symbol, Unit auto U>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const named_unit<Symbol, U>&)
{
  return get_canonical_unit_impl(U, U);
}

template<typename T, typename F, int Num, int... Den>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const power<F, Num, Den...>&)
{
  auto base = get_canonical_unit_impl(F{}, F{});
  return canonical_unit{
    pow<power<F, Num, Den...>::exponent>(base.mag),
    derived_unit<power_or_T<std::remove_const_t<decltype(base.reference_unit)>, power<F, Num, Den...>::exponent>>{}};
}

template<Unit T, typename... Us>
[[nodiscard]] consteval auto get_canonical_unit_impl(T, const derived_unit<Us...>&)
{
  if constexpr (type_list_size<typename derived_unit<Us...>::_den_> != 0) {
    using num_type = type_list_map<typename derived_unit<Us...>::_num_, derived_unit>;
    using den_type = type_list_map<typename derived_unit<Us...>::_den_, derived_unit>;
    auto num = get_canonical_unit_impl(num_type{}, num_type{});
    auto den = get_canonical_unit_impl(den_type{}, den_type{});
    return canonical_unit{num.mag / den.mag, num.reference_unit / den.reference_unit};
  } else {
    auto num = (one * ... * get_canonical_unit_impl(Us{}, Us{}).reference_unit);
    auto mag = (units::mag<1> * ... * get_canonical_unit_impl(Us{}, Us{}).mag);
    return canonical_unit{mag, num};
  }
}

[[nodiscard]] consteval auto get_canonical_unit(Unit auto u) { return get_canonical_unit_impl(u, u); }

// TODO What if the same unit will have different types (i.e. user will inherit its own type from `metre`)?
// Is there a better way to sort units here? Some of them may not have symbol at all (like all units of
// dimensionless quantities).
template<Unit Lhs, Unit Rhs>
struct unit_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

template<typename T1, typename T2>
using type_list_of_unit_less = expr_less<T1, T2, unit_less>;

}  // namespace detail


// Operators

/**
 * Multiplication by `1` returns the same unit, otherwise `scaled_unit` is being returned.
 */
template<Magnitude M, Unit U>
[[nodiscard]] consteval Unit auto operator*(M mag, const U u)
{
  if constexpr (mag == units::mag<1>)
    return u;
  else
    return scaled_unit<mag, U>{};
}

template<Magnitude M, Unit U>
[[nodiscard]] consteval Unit auto operator*(U, M) = delete;

/**
 * `scaled_unit` specializations have priority in this operation. This means that the library framework
 * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
 * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
 */
template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval Unit auto operator*(Lhs lhs, Rhs rhs)
{
  if constexpr (detail::is_specialization_of_scaled_unit<Lhs> && detail::is_specialization_of_scaled_unit<Rhs>)
    return (Lhs::mag * Rhs::mag) * (Lhs::reference_unit * Rhs::reference_unit);
  else if constexpr (detail::is_specialization_of_scaled_unit<Lhs>)
    return Lhs::mag * (Lhs::reference_unit * rhs);
  else if constexpr (detail::is_specialization_of_scaled_unit<Rhs>)
    return Rhs::mag * (lhs * Rhs::reference_unit);
  else
    return detail::expr_multiply<derived_unit, struct one, detail::type_list_of_unit_less>(lhs, rhs);
}

/**
 * `scaled_unit` specializations have priority in this operation. This means that the library framework
 * prevents passing it as an element to the `derived_unit`. In such case only the reference unit is passed
 * to the derived unit and the magnitude remains outside forming another scaled unit as a result of the operation.
 */
template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval Unit auto operator/(Lhs lhs, Rhs rhs)
{
  if constexpr (detail::is_specialization_of_scaled_unit<Lhs> && detail::is_specialization_of_scaled_unit<Rhs>)
    return (Lhs::mag / Rhs::mag) * (Lhs::reference_unit / Rhs::reference_unit);
  else if constexpr (detail::is_specialization_of_scaled_unit<Lhs>)
    return Lhs::mag * (Lhs::reference_unit / rhs);
  else if constexpr (detail::is_specialization_of_scaled_unit<Rhs>)
    return Rhs::mag * (lhs / Rhs::reference_unit);
  else
    return detail::expr_divide<derived_unit, struct one, detail::type_list_of_unit_less>(lhs, rhs);
}

template<Unit U>
[[nodiscard]] consteval Unit auto operator/(int value, U u)
{
  gsl_Expects(value == 1);
  return detail::expr_invert<derived_unit, struct one>(u);
}

template<Unit U>
[[nodiscard]] consteval Unit auto operator/(U, int) = delete;

template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval bool operator==(Lhs lhs, Rhs rhs)
{
  auto canonical_lhs = detail::get_canonical_unit(lhs);
  auto canonical_rhs = detail::get_canonical_unit(rhs);
  return is_same_v<decltype(canonical_lhs.reference_unit), decltype(canonical_rhs.reference_unit)> &&
         canonical_lhs.mag == canonical_rhs.mag;
}


// Convertible
template<Unit Lhs, Unit Rhs>
[[nodiscard]] consteval bool convertible(Lhs lhs, Rhs rhs)
{
  auto canonical_lhs = detail::get_canonical_unit(lhs);
  auto canonical_rhs = detail::get_canonical_unit(rhs);
  return is_same_v<decltype(canonical_lhs.reference_unit), decltype(canonical_rhs.reference_unit)>;
}


/**
 * @brief Computes the value of a unit raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param u Unit being the base of the operation
 *
 * @return Unit The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Unit U>
  requires detail::non_zero<Den>
[[nodiscard]] consteval Unit auto pow(U u)
{
  if constexpr (requires { U::symbol; }) {
    if constexpr (Den == 1)
      return derived_unit<power<U, Num>>{};
    else
      return derived_unit<power<U, Num, Den>>{};
  } else if constexpr (detail::is_specialization_of_scaled_unit<U>) {
    return scaled_unit<pow<ratio{Num, Den}>(U::mag), std::remove_const_t<decltype(pow<Num, Den>(U::reference_unit))>>{};
  } else {
    return detail::expr_pow<Num, Den, derived_unit, struct one, detail::type_list_of_unit_less>(u);
  }
}


// Helper variable templates to create common powers
template<Unit auto U>
inline constexpr decltype(U * U) square;

template<Unit auto U>
inline constexpr decltype(U * U * U) cubic;


// get_unit_symbol

enum class text_encoding {
  unicode,  // m³;  µs
  ascii,    // m^3; us
  default_encoding = unicode
};

enum class unit_symbol_denominator {
  solidus_one,      // m/s;   kg m-1 s-1
  always_solidus,   // m/s;   kg/(m s)
  always_negative,  // m s-1; kg m-1 s-1
  default_denominator = solidus_one
};

enum class unit_symbol_separator {
  space,  // kg m²/s²
  dot,    // kg⋅m²/s²  (valid only for unicode encoding)
  default_separator = space
};

struct unit_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
  unit_symbol_denominator denominator = unit_symbol_denominator::default_denominator;
  unit_symbol_separator separator = unit_symbol_separator::default_separator;
};

namespace detail {

// TODO Should `basic_symbol_text` be fixed to use `char` type for both encodings?
template<typename CharT, typename UnicodeCharT, std::size_t N, std::size_t M, std::output_iterator<CharT> Out>
constexpr Out copy(const basic_symbol_text<UnicodeCharT, N, M>& txt, text_encoding encoding, Out out)
{
  if (encoding == text_encoding::unicode) {
    if (is_same_v<CharT, UnicodeCharT>)
      return copy(txt.unicode(), out).out;
    else
      throw std::invalid_argument("Unicode text can't be copied to CharT output");
  } else {
    if (is_same_v<CharT, char>)
      return copy(txt.ascii(), out).out;
    else
      throw std::invalid_argument("ASCII text can't be copied to CharT output");
  }
}

inline constexpr basic_symbol_text base_multiplier("\u00D7 10", "x 10");

template<Magnitude auto M>
constexpr auto magnitude_text()
{
  constexpr auto exp10 = extract_power_of_10(M);

  constexpr Magnitude auto base = M / mag_power<10, exp10>;
  constexpr Magnitude auto num = numerator(base);
  constexpr Magnitude auto den = denominator(base);
  static_assert(base == num / den, "Printing rational powers, or irrational bases, not yet supported");

  constexpr auto num_value = get_value<std::intmax_t>(num);
  constexpr auto den_value = get_value<std::intmax_t>(den);

  if constexpr (num_value == 1 && den_value == 1 && exp10 != 0) {
    return base_multiplier + superscript<exp10>();
  } else if constexpr (num_value != 1 || den_value != 1 || exp10 != 0) {
    auto txt = basic_fixed_string("[") + regular<num_value>();
    if constexpr (den_value == 1) {
      if constexpr (exp10 == 0) {
        return txt + basic_fixed_string("]");
      } else {
        return txt + " " + base_multiplier + superscript<exp10>() + basic_fixed_string("]");
      }
    } else {
      if constexpr (exp10 == 0) {
        return txt + basic_fixed_string("/") + regular<den_value>() + basic_fixed_string("]");
      } else {
        return txt + basic_fixed_string("/") + regular<den_value>() + " " + base_multiplier + superscript<exp10>() +
               basic_fixed_string("]");
      }
    }
  } else {
    return basic_fixed_string("");
  }
}

template<typename CharT, std::output_iterator<CharT> Out>
constexpr Out print_separator(Out out, unit_symbol_formatting fmt)
{
  if (fmt.separator == unit_symbol_separator::dot) {
    if (fmt.encoding != text_encoding::unicode)
      throw std::invalid_argument("'unit_symbol_separator::dot' can be only used with 'text_encoding::unicode'");
    copy(std::string_view("⋅"), out);
  } else {
    *out++ = ' ';
  }
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, Unit U>
  requires requires { U::symbol; }
constexpr Out unit_symbol_impl(Out out, U, unit_symbol_formatting fmt, bool negative_power)
{
  out = copy<CharT>(U::symbol, fmt.encoding, out);
  if (negative_power) {
    constexpr auto txt = superscript<-1>();
    out = copy<CharT>(txt, fmt.encoding, out);
  }
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, auto M, typename U>
constexpr Out unit_symbol_impl(Out out, const scaled_unit<M, U>& u, unit_symbol_formatting fmt, bool negative_power)
{
  if constexpr (M == mag<1>) {
    // no ratio/prefix
    return unit_symbol_impl<CharT>(out, u.reference_unit, fmt, negative_power);
  } else {
    constexpr auto mag_txt = magnitude_text<M>();
    out = copy<CharT>(mag_txt, fmt.encoding, out);

    if constexpr (std::derived_from<std::remove_const_t<decltype(u.reference_unit)>, derived_unit<>>)
      return out;
    else {
      *out++ = ' ';
      return unit_symbol_impl<CharT>(out, u.reference_unit, fmt, negative_power);
    }
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
constexpr auto unit_symbol_impl(Out out, const power<F, Num, Den...>&, unit_symbol_formatting fmt, bool negative_power)
{
  out = unit_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed

  constexpr ratio r = power<F, Num, Den...>::exponent;
  if constexpr (r.den != 1) {
    // add root part
    constexpr auto txt = basic_fixed_string("^(") + regular<r.num>() + basic_fixed_string("/") + regular<r.den>() +
                         basic_fixed_string(")");
    return copy<CharT>(txt, fmt.encoding, out);
  } else if constexpr (r.num != 1) {
    // add exponent part
    if (negative_power) {
      constexpr auto txt = superscript<-r.num>();
      return copy<CharT>(txt, fmt.encoding, out);
    } else {
      constexpr auto txt = superscript<r.num>();
      return copy<CharT>(txt, fmt.encoding, out);
    }
  }
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitSpec M>
constexpr Out unit_symbol_impl(Out out, M m, std::size_t Idx, unit_symbol_formatting fmt, bool negative_power)
{
  if (Idx > 0) out = print_separator<CharT>(out, fmt);
  return unit_symbol_impl<CharT>(out, m, fmt, negative_power);
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitSpec... Ms, std::size_t... Idxs>
constexpr Out unit_symbol_impl(Out out, const type_list<Ms...>&, std::index_sequence<Idxs...>,
                               unit_symbol_formatting fmt, bool negative_power)
{
  return (..., (out = unit_symbol_impl<CharT>(out, Ms{}, Idxs, fmt, negative_power)));
}

template<typename CharT, std::output_iterator<CharT> Out, DerivedUnitSpec... Nums, DerivedUnitSpec... Dens>
constexpr Out unit_symbol_impl(Out out, const type_list<Nums...>& nums, const type_list<Dens...>& dens,
                               unit_symbol_formatting fmt)
{
  if constexpr (sizeof...(Nums) == 0 && sizeof...(Dens) == 0) {
    // dimensionless quantity
    return out;
  } else if constexpr (sizeof...(Dens) == 0) {
    // no denominator
    return unit_symbol_impl<CharT>(out, nums, std::index_sequence_for<Nums...>(), fmt, false);
  } else {
    using enum unit_symbol_denominator;
    if constexpr (sizeof...(Nums) > 0) {
      unit_symbol_impl<CharT>(out, nums, std::index_sequence_for<Nums...>(), fmt, false);
    }

    if (fmt.denominator == always_solidus || (fmt.denominator == solidus_one && sizeof...(Dens) == 1)) {
      if constexpr (sizeof...(Nums) == 0) *out++ = '1';
      *out++ = '/';
    } else {
      out = print_separator<CharT>(out, fmt);
    }

    if (fmt.denominator == always_solidus && sizeof...(Dens) > 1) *out++ = '(';
    bool negative_power = fmt.denominator == always_negative || (fmt.denominator == solidus_one && sizeof...(Dens) > 1);
    out = unit_symbol_impl<CharT>(out, dens, std::index_sequence_for<Dens...>(), fmt, negative_power);
    if (fmt.denominator == always_solidus && sizeof...(Dens) > 1) *out++ = ')';
    return out;
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Us>
constexpr Out unit_symbol_impl(Out out, const derived_unit<Us...>&, unit_symbol_formatting fmt, bool negative_power)
{
  gsl_Expects(negative_power == false);
  return unit_symbol_impl<CharT>(out, typename derived_unit<Us...>::_num_{}, typename derived_unit<Us...>::_den_{},
                                 fmt);
}

}  // namespace detail


template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, unit_symbol_formatting fmt = unit_symbol_formatting{})
{
  return detail::unit_symbol_impl<CharT>(out, u, fmt, false);
}

template<typename CharT = char, Unit U>
[[nodiscard]] constexpr std::basic_string<CharT> unit_symbol(U u, unit_symbol_formatting fmt = unit_symbol_formatting{})
{
  std::basic_string<CharT> buffer;
  unit_symbol_to<CharT>(std::back_inserter(buffer), u, fmt);
  return buffer;
}

namespace detail {


template<typename U1, typename U2>
[[nodiscard]] consteval auto common_type_impl(const U1 u1, const U2 u2)
{
  if constexpr (U1{} == U2{}) {
    if constexpr (std::derived_from<U1, U2>)
      return u1;
    else
      return u2;
  } else {
    constexpr auto canonical_lhs = detail::get_canonical_unit(U1{});
    constexpr auto canonical_rhs = detail::get_canonical_unit(U2{});

    if constexpr (is_integral(canonical_lhs.mag / canonical_rhs.mag))
      return u2;
    else if constexpr (is_integral(canonical_rhs.mag / canonical_lhs.mag))
      return u1;
    else {
      constexpr auto cm = common_magnitude(canonical_lhs.mag, canonical_rhs.mag);
      return scaled_unit<cm, std::remove_const_t<decltype(canonical_lhs.reference_unit)>>{};
    }
  }
}

}  // namespace detail

}  // namespace units

namespace std {

template<units::Unit U1, units::Unit U2>
  requires(units::convertible(U1{}, U2{}))
struct common_type<U1, U2> {
  using type = std::remove_const_t<decltype(::units::detail::common_type_impl(U1{}, U2{}))>;
};

}  // namespace std
