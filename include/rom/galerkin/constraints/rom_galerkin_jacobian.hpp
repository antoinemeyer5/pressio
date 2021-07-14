/*
//@HEADER
// ************************************************************************
//
// rom_galerkin_jacobian.hpp
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

#ifndef ROM_GALERKIN_CONSTRAINTS_ROM_GALERKIN_JACOBIAN_HPP_
#define ROM_GALERKIN_CONSTRAINTS_ROM_GALERKIN_JACOBIAN_HPP_

namespace pressio { namespace rom { namespace galerkin { namespace constraints {

template <typename T, typename enable = void>
struct jacobian : std::false_type
{
};

#ifdef PRESSIO_ENABLE_TPL_EIGEN
template <typename T>
struct jacobian<
  T,
  ::pressio::mpl::enable_if_t<
    ::pressio::containers::predicates::is_dense_matrix_wrapper_eigen<T>::value>> : std::true_type
{
};
#endif

#ifdef PRESSIO_ENABLE_TPL_KOKKOS
template <typename T>
struct jacobian<
  T,
  ::pressio::mpl::enable_if_t<
    ::pressio::containers::predicates::is_dense_matrix_wrapper_kokkos<T>::value>> : std::true_type
{
};
#endif

#ifdef PRESSIO_ENABLE_TPL_PYBIND11
template <typename T>
struct jacobian<
  T,
  ::pressio::mpl::enable_if_t<
    ::pressio::containers::predicates::is_fstyle_rank2_tensor_wrapper_pybind<T>::value>> : std::true_type
{
};
#endif

template <typename T>
using galerkin_jacobian = jacobian<T>;

}}}}// namespace pressio::ode::constraints
#endif// ROM_GALERKIN_CONSTRAINTS_ROM_GALERKIN_JACOBIAN_HPP_
