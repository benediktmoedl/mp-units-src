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

#include <units/physical/dimensions/acceleration.h>
#include <units/physical/si/fps/derived/speed.h>
#include <units/quantity.h>

namespace units::physical::si::fps {

struct foot_per_second_sq : unit<foot_per_second_sq> {};
struct dim_acceleration : physical::dim_acceleration<dim_acceleration, foot_per_second_sq, dim_length, dim_time> {};

template<UnitOf<dim_acceleration> U, QuantityValue Rep = double>
using acceleration = quantity<dim_acceleration, U, Rep>;

inline namespace literals {

// ft_per_s2
constexpr auto operator"" _q_ft_per_s2(unsigned long long l) { return acceleration<foot_per_second_sq, std::int64_t>(l); }
constexpr auto operator"" _q_ft_per_s2(long double l) { return acceleration<foot_per_second_sq, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si::fps
