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

#include "bits/tools.h"
#include "dimension.h"

namespace units {

  template<Dimension D, Ratio R>
  struct unit {
    using dimension = D;
    using ratio = R;
    static_assert(ratio::num > 0, "ratio must be positive");
  };

  // is_unit

  namespace detail {

    template<typename T>
    struct is_unit : std::false_type {
    };

    template<Dimension D, Ratio R>
    struct is_unit<unit<D, R>> : std::true_type {
    };

  }

  template<typename T>
  concept bool Unit = detail::is_unit<T>::value;

//  template<Unit U1, Unit U2>
//  auto operator/(U1, U2)
//  {
//    return ;
//  }
//
//  unit<dimension_divide_t<D1, D2>, std::ratio_divide<typename U1::ratio, typename::U2::ratio>>

}  // namespace units
