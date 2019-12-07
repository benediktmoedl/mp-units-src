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

#include <units/physical/dimensions.h>
#include <units/physical/si/length.h>
#include <units/quantity.h>

namespace units::si {

struct cubic_metre : unit<cubic_metre> {};
struct dim_volume : physical::dim_volume<dim_volume, cubic_metre, dim_length> {};

struct cubic_millimetre : deduced_unit<cubic_millimetre, dim_volume, millimetre> {};
struct cubic_centimetre : deduced_unit<cubic_centimetre, dim_volume, centimetre> {};
struct cubic_kilometre : deduced_unit<cubic_kilometre, dim_volume, kilometre> {};
struct cubic_foot : deduced_unit<cubic_foot, dim_volume, foot> {};

template<Unit U, Scalar Rep = double>
using volume = quantity<dim_volume, U, Rep>;

inline namespace literals {

// cub_mm
constexpr auto operator""cub_mm(unsigned long long l) { return volume<cubic_millimetre, std::int64_t>(l); }
constexpr auto operator""cub_mm(long double l) { return volume<cubic_millimetre, long double>(l); }

// cub_cm
constexpr auto operator""cub_cm(unsigned long long l) { return volume<cubic_centimetre, std::int64_t>(l); }
constexpr auto operator""cub_cm(long double l) { return volume<cubic_centimetre, long double>(l); }

// cub_m
constexpr auto operator""cub_m(unsigned long long l) { return volume<cubic_metre, std::int64_t>(l); }
constexpr auto operator""cub_m(long double l) { return volume<cubic_metre, long double>(l); }

// cub_km
constexpr auto operator""cub_km(unsigned long long l) { return volume<cubic_kilometre, std::int64_t>(l); }
constexpr auto operator""cub_km(long double l) { return volume<cubic_kilometre, long double>(l); }

// cub_ft
constexpr auto operator""cub_ft(unsigned long long l) { return volume<cubic_foot, std::int64_t>(l); }
constexpr auto operator""cub_ft(long double l) { return volume<cubic_foot, long double>(l); }

}  // namespace literals

}  // namespace units::si
