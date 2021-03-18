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

#include <units/isq/dimensions/dynamic_viscosity.h>
#include <units/isq/si/time.h>
#include <units/isq/si/pressure.h>
#include <units/quantity.h>

namespace units::isq::si {

struct pascal_second : unit<pascal_second> {};
struct dim_dynamic_viscosity : isq::dim_dynamic_viscosity<dim_dynamic_viscosity, pascal_second, dim_pressure, dim_time> {};

template<UnitOf<dim_dynamic_viscosity> U, QuantityValue Rep = double>
using dynamic_viscosity = quantity<dim_dynamic_viscosity, U, Rep>;

inline namespace literals {

// Pa·s
constexpr auto operator"" _q_Pa_s(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return dynamic_viscosity<pascal_second, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_Pa_s(long double l) { return dynamic_viscosity<pascal_second, long double>(l); }

}  // namespace literals

namespace references {

inline constexpr auto Pa_s = reference<dim_dynamic_viscosity, pascal_second>{};

}  // namespace references

}  // namespace units::isq::si

