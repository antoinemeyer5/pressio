/*
//@HEADER
// ************************************************************************
//
// ode_implicit_discrete_time_residual_policy.hpp
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

#ifndef ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_RESIDUAL_POLICY_HPP_
#define ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_RESIDUAL_POLICY_HPP_

namespace pressio { namespace ode { namespace implicitmethods { namespace policy {

template <typename state_type, typename residual_type>
class ResidualStandardDiscreteTimePolicy
{
  static_assert(::pressio::ode::constraints::implicit_state<state_type>::value,
		"Invalid state type for standard residual policy");

  static_assert(::pressio::ode::constraints::implicit_residual<residual_type>::value,
		"Invalid residual type for standard residual policy");

public:
  ResidualStandardDiscreteTimePolicy() = default;
  ResidualStandardDiscreteTimePolicy(const ResidualStandardDiscreteTimePolicy &) = default;
  ResidualStandardDiscreteTimePolicy & operator=(const ResidualStandardDiscreteTimePolicy &) = default;
  ResidualStandardDiscreteTimePolicy(ResidualStandardDiscreteTimePolicy &&) = default;
  ResidualStandardDiscreteTimePolicy & operator=(ResidualStandardDiscreteTimePolicy &&) = default;
  ~ResidualStandardDiscreteTimePolicy() = default;

public:
  template <typename system_type>
  residual_type create(const system_type & system) const
  {
    static_assert(::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<system_type>::value,
		  "system type must meet the discrete time api");

    residual_type R(system.createDiscreteTimeResidual());
    return R;
  }

  //-------------------------------
  // 1 aux state needed
  //-------------------------------
  template <class ode_tag, class stencil_states_type, class system_type, class scalar_type>
  mpl::enable_if_t<stencil_states_type::size() == 1>
  compute(const state_type & predictedState,
	  const stencil_states_type & stencilStatesManager,
	  const system_type & system,
	  const scalar_type & rhsEvaluationTime,
	  const scalar_type & dt,
	  const types::step_t & step,
	  residual_type & R) const
  {
    static_assert(::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<system_type>::value,
		  "system type must meet the discrete time api");

    const auto & yn = stencilStatesManager.stateAt(ode::n());

    try {
      system.template discreteTimeResidual(step, rhsEvaluationTime, dt, *R.data(),
					   *predictedState.data(),
					   *yn.data());
    } catch(::pressio::eh::discrete_time_residual_failure_unrecoverable const & e) {
      throw ::pressio::eh::residual_evaluation_failure_unrecoverable();
    }
  }

  //-------------------------------
  // 2 aux states needed
  //-------------------------------
  template <class ode_tag, class stencil_states_type, class system_type, class scalar_type>
  mpl::enable_if_t<stencil_states_type::size() == 2>
  compute(const state_type & predictedState,
	  const stencil_states_type & stencilStatesManager,
	  const system_type & system,
	  const scalar_type & rhsEvaluationTime,
	  const scalar_type & dt,
	  const types::step_t & step,
	  residual_type & R) const
  {
    static_assert(::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<system_type>::value,
		  "system type must meet the discrete time api");

    const auto & yn = stencilStatesManager.stateAt(ode::n());
    const auto & ynm1 = stencilStatesManager.stateAt(ode::nMinusOne());

    try {
      system.template discreteTimeResidual(step, rhsEvaluationTime, dt, *R.data(),
					   *predictedState.data(),
					   *yn.data(), *ynm1.data());
    } catch(::pressio::eh::discrete_time_residual_failure_unrecoverable const & e) {
      throw ::pressio::eh::residual_evaluation_failure_unrecoverable();
    }
  }

  //-------------------------------
  // 3 aux states needed
  //-------------------------------
  template <class ode_tag, class stencil_states_type, class system_type, class scalar_type>
  mpl::enable_if_t<stencil_states_type::size() == 3>
  compute(const state_type & predictedState,
	  const stencil_states_type & stencilStatesManager,
	  const system_type & system,
	  const scalar_type & rhsEvaluationTime,
	  const scalar_type & dt,
	  const types::step_t & step,
	  residual_type & R) const
  {
    static_assert(::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<system_type>::value,
		  "system type must meet the discrete time api");

    const auto & yn = stencilStatesManager.stateAt(ode::n());
    const auto & ynm1 = stencilStatesManager.stateAt(ode::nMinusOne());
    const auto & ynm2 = stencilStatesManager.stateAt(ode::nMinusTwo());

    try {
      system.template discreteTimeResidual(step, rhsEvaluationTime, dt, *R.data(),
					   *predictedState.data(),
					   *yn.data(),
					   *ynm1.data(),
					   *ynm2.data());
    } catch(::pressio::eh::discrete_time_residual_failure_unrecoverable const & e) {
      throw ::pressio::eh::residual_evaluation_failure_unrecoverable();
    }
  }
};

}}}}//end namespace pressio::ode::implicitmethods::policy
#endif// ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_RESIDUAL_POLICY_HPP_
