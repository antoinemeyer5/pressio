/*
//@HEADER
// ************************************************************************
//
// rom_galerkin.hpp
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

#ifndef PRESSIO_ROM_CONCEPTS_TOPLEVEL_INC_HPP_
#define PRESSIO_ROM_CONCEPTS_TOPLEVEL_INC_HPP_

#include "./mpl.hpp"
#include "./utils.hpp"
#include "./type_traits.hpp"
#include "./ops.hpp"
#include "./qr.hpp"
#include "./solvers_linear.hpp"
#include "./solvers_nonlinear.hpp"
#include "./ode.hpp"

#ifdef PRESSIO_ENABLE_CXX20
#include <concepts>
#endif

#include "./rom/predicates.hpp"
#include "./rom/reduced_operators_traits.hpp"
#include "./rom/concepts/valid_reduced_state.hpp"
#include "./rom/concepts/linear_subspace.hpp"
#include "./rom/concepts/possibly_affine_trial_column_subspace.hpp"
#include "./rom/concepts/steady_fom_with_jac_action.hpp"
#include "./rom/concepts/steady_galerkin_projectable_on_affine_subspace.hpp"
#include "./rom/concepts/steady_galerkin_hyperreduceable_with.hpp"
#include "./rom/concepts/steady_galerkin_hyperreduceable_maskable_with.hpp"
#include "./rom/concepts/steady_lspg_maskable_with.hpp"
#include "./rom/concepts/semi_discrete_fom.hpp"
#include "./rom/concepts/semi_discrete_fom_with_jac_action.hpp"

#include "./rom/concepts/unsteady_lspg_default.hpp"
#include "./rom/concepts/unsteady_lspg_hypred.hpp"
#include "./rom/concepts/unsteady_lspg_masked.hpp"
#include "./rom/concepts/fully_discrete_with_jac_action.hpp"

#endif
