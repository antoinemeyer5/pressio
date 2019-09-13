/*
//@HEADER
// ************************************************************************
//
// solvers_system_has_all_needed_jacobian_methods.hpp
//                     		  Pressio
//                             Copyright 2019
//    National Technology & Engineering Solutions of Sandia, LLC (NTESS)
//
// Under the terms of Contract DE-NA0003525 with NTESS, the
// U.S. Government retains certain rights in this software.
//
// Pressio is licensed under BSD-3-Clause terms of use:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Francesco Rizzi (fnrizzi@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef SOLVERS_SYSTEM_HAS_ALL_NEEDED_JACOBIAN_METHODS_HPP_
#define SOLVERS_SYSTEM_HAS_ALL_NEEDED_JACOBIAN_METHODS_HPP_

#include "../solvers_ConfigDefs.hpp"
#include "../../../mpl/src/detection_idiom.hpp"

namespace pressio{ namespace solvers{ namespace meta {

template <typename T, typename Arg, typename enable = void>
struct has_jacobian_method_callable_with_one_arg{
  using type = void;
  static constexpr bool value = false;
};

template <typename T, typename Arg>
struct has_jacobian_method_callable_with_one_arg<
  T, Arg,
  ::pressio::mpl::void_t<
    decltype(std::declval<T>().jacobian(std::declval<Arg const&>()))
    >
  >{
  using type = decltype(std::declval<T>().jacobian(std::declval<Arg const&>()));
  static constexpr bool value = true;
};


template <typename T, typename FirstArg, typename SecondArg, typename = void>
struct has_jacobian_method_callable_with_two_args : std::false_type{};

template <typename T, typename FirstArg, typename SecondArg>
struct has_jacobian_method_callable_with_two_args<
  T, FirstArg, SecondArg,
  ::pressio::mpl::void_t<
    decltype(std::declval<T>().jacobian(std::declval<FirstArg const&>(),
					std::declval<SecondArg&>()))
    >
  > : std::true_type{};



template<
  typename system_type,
  typename state_type,
  typename jacobian_type,
  typename enable = void
  >
struct system_has_needed_jacobian_methods : std::false_type{};

template<
  typename system_type,
  typename state_type,
  typename jacobian_type
  >
struct system_has_needed_jacobian_methods
< system_type, state_type, jacobian_type,
  ::pressio::mpl::enable_if_t<
      has_jacobian_method_callable_with_one_arg<
	system_type, state_type
      >::value and
   // has method with 2 arguments,
      has_jacobian_method_callable_with_two_args<
	system_type, state_type, jacobian_type
      >::value and
    // method with 1 argument returns a jacobian_type
    std::is_same<
	typename has_jacobian_method_callable_with_one_arg<
	  system_type, state_type>::type,
      jacobian_type
      >::value
  >
  > : std::true_type{};

}}} // namespace pressio::solvers::meta
#endif
