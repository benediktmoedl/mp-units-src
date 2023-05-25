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

#include <mp-units/customization_points.h>
#include <concepts>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace mp_units {

/**
 * @brief Quantity character
 *
 * Scalars, vectors and tensors are mathematical objects that can be used to
 * denote certain physical quantities and their values. They are as such
 * independent of the particular choice of a coordinate system, whereas
 * each scalar component of a vector or a tensor and each component vector and
 * component tensor depend on that choice.
 *
 * A scalar is a physical quantity that has magnitude but no direction.
 *
 * Vectors are physical quantities that possess both magnitude and direction
 * and whose operations obey the axioms of a vector space.
 *
 * Tensors can be used to describe more general physical quantities.
 * For example, the Cauchy stress tensor possess magnitude, direction,
 * and orientation qualities.
 */
enum class quantity_character { scalar, vector, tensor };

template<typename T, typename U>
concept common_type_with_ =  // exposition only
  std::same_as<std::common_type_t<T, U>, std::common_type_t<U, T>> &&
  std::constructible_from<std::common_type_t<T, U>, T> && std::constructible_from<std::common_type_t<T, U>, U>;

template<typename T, typename U = T>
concept scalable_number_ =  // exposition only
  std::regular_invocable<std::multiplies<>, T, U> && std::regular_invocable<std::divides<>, T, U>;

template<typename T>
concept castable_number_ =  // exposition only
  common_type_with_<T, std::intmax_t> && scalable_number_<std::common_type_t<T, std::intmax_t>>;

// TODO Fix it according to sudo_cast implementation
template<typename T>
concept scalable_ =  // exposition only
  castable_number_<T> || (requires { typename T::value_type; } && castable_number_<typename T::value_type> &&
                          scalable_number_<T, std::common_type_t<typename T::value_type, std::intmax_t>>);

template<typename T>
concept Representation = (is_scalar<T> || is_vector<T> || is_tensor<T>)&&std::regular<T> && scalable_<T>;

template<typename T, quantity_character Ch>
concept RepresentationOf = Representation<T> && ((Ch == quantity_character::scalar && is_scalar<T>) ||
                                                 (Ch == quantity_character::vector && is_vector<T>) ||
                                                 (Ch == quantity_character::tensor && is_tensor<T>));

}  // namespace mp_units
