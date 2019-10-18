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

#include <units/unit.h>
#include <ostream>
#include <string_view>

namespace units {

  namespace detail {

    template<typename Ratio, typename CharT, typename Traits>
    void print_ratio(std::basic_ostream<CharT, Traits>& os)
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        if constexpr(Ratio::den == 1) {
          os << "[" << Ratio::num << "]";
        }
        else {
          os << "[" << Ratio::num << "/" << Ratio::den << "]";
        }
      }
    }

    template<typename Ratio, typename PrefixType, typename CharT, typename Traits>
    void print_prefix_or_ratio(std::basic_ostream<CharT, Traits>& os)
    {
      if constexpr(Ratio::num != 1 || Ratio::den != 1) {
        if(!std::same_as<PrefixType, no_prefix>) {
          using prefix = downcast_target<detail::prefix_base<PrefixType, Ratio>>;

          if constexpr(!std::same_as<prefix, prefix_base<PrefixType, Ratio>>) {
            // print as a prefixed unit
            os << prefix::symbol;
            return;
          }
        }
        // print as a ratio of the coherent unit
        print_ratio<Ratio>(os);
      }
    }

    template<typename CharT, typename Traits, typename... Es>
    void print_dimensions(std::basic_ostream<CharT, Traits>& os, dimension<Es...>)
    {
      bool first = true;
      auto ingr_printer = [&]<typename E>(E) {
        if constexpr(E::num < 0) {
          os << (first ? "1/" : "/");
        }
        else {
          os << (first ? "" : "⋅");
        }
        os << E::dimension::symbol;
        if constexpr(E::den != 1) {
          os << "^(" << abs(E::num) << "/" << E::den << ")";
        }
        else if constexpr(abs(E::num) != 1) {
          // if constexpr(is_unicode<CharT>)
          //   os << superscript<abs(E::num)>;
          // else
            os << "^" << abs(E::num);
        }
        first = false;
      };
      (ingr_printer(Es{}), ...);
    }

  }

}  // namespace units

// template<Unit U, Scalar Rep, typename CharT>
// struct fmt::formatter<units::quantity<U, Rep>, CharT> {
//   template<typename ParseContext>
//   constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

//   template<typename FormatContext>
//   auto format(const units::quantity<U, Rep>& q, FormatContext& ctx)
//   {
//     return format_to(ctx.out(), "{:.1f}, {:.1f}", p.x, p.y);
//   }
// }
