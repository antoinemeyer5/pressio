/*
//@HEADER
// ************************************************************************
//
// ode_implicit_discrete_time_jacobian_policy.hpp
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

#ifndef ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_JACOBIAN_POLICY_HPP_
#define ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_JACOBIAN_POLICY_HPP_

namespace pressio{ namespace ode{ namespace implicitmethods{ namespace policy{

template<typename state_type, typename jacobian_type>
class JacobianStandardDiscreteTimePolicy
{
  static_assert
  (::pressio::ode::constraints::implicit_state<state_type>::value,
   "Invalid state type for standard jacobian policy");

  static_assert
  (::pressio::ode::constraints::implicit_jacobian<jacobian_type>::value,
   "Invalid jacobian type for standard jacobian policy");

public:
  JacobianStandardDiscreteTimePolicy() = default;
  JacobianStandardDiscreteTimePolicy(const JacobianStandardDiscreteTimePolicy &) = default;
  JacobianStandardDiscreteTimePolicy & operator=(const JacobianStandardDiscreteTimePolicy &) = default;
  JacobianStandardDiscreteTimePolicy(JacobianStandardDiscreteTimePolicy &&) = default;
  JacobianStandardDiscreteTimePolicy & operator=(JacobianStandardDiscreteTimePolicy &&) = default;
  ~JacobianStandardDiscreteTimePolicy() = default;

public:
  template <typename system_type>
  jacobian_type create(const system_type & system) const
  {
    static_assert
      (::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<
       system_type>::value, "system type must meet the discrete time api");

    jacobian_type JJ(system.createDiscreteTimeJacobian());
    return JJ;
  }

  //-------------------------------
  // specialize for n == 1 aux states
  //-------------------------------
  template <
    typename ode_tag,
    typename stencil_data_type,
    typename system_type,
    typename scalar_type
    >
  mpl::enable_if_t<stencil_data_type::size()==1>
  compute(const state_type & odeCurrentState,
	  const stencil_data_type & stencilDataManager,
	  const system_type & system,
	  const scalar_type & t,
	  const scalar_type & dt,
	  const types::step_t &  step,
	  jacobian_type & J) const
  {
    static_assert
      (::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<
       system_type>::value, "system type must meet the discrete time api");

    const auto & yn = stencilDataManager.stateAt(ode::n());
    system.template discreteTimeJacobian(step, t, dt, *J.data(),
					 *odeCurrentState.data(),
					 *yn.data() );
  }

  //-------------------------------
  // specialize for n == 2 aux states
  //-------------------------------
  template <
    typename ode_tag, typename stencil_data_type,
    typename system_type, typename scalar_type
    >
  mpl::enable_if_t<stencil_data_type::size()==2>
  compute(const state_type & odeCurrentState,
	  const stencil_data_type & stencilDataManager,
	  const system_type & system,
	  const scalar_type & t,
	  const scalar_type & dt,
	  const types::step_t & step,
	  jacobian_type & J) const
  {
    static_assert
      (::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<
       system_type>::value, "system type must meet the discrete time api");

    const auto & yn = stencilDataManager.stateAt(ode::n());
    const auto & ynm1 = stencilDataManager.stateAt(ode::nMinusOne());
    system.template discreteTimeJacobian(step, t, dt, *J.data(),
					 *odeCurrentState.data(),
					 (*yn.data() ), (*ynm1.data()) );
  }

  //-------------------------------
  // specialize for n == 3 aux states
  //-------------------------------
  template <
    typename ode_tag, typename stencil_data_type,
    typename system_type, typename scalar_type
    >
  mpl::enable_if_t<stencil_data_type::size()==3>
  compute(const state_type & odeCurrentState,
	  const stencil_data_type & stencilDataManager,
	  const system_type & system,
	  const scalar_type & t,
	  const scalar_type & dt,
	  const types::step_t & step,
	  jacobian_type & J) const
  {
    static_assert
      (::pressio::ode::constraints::discrete_time_system_with_user_provided_jacobian<
       system_type>::value, "system type must meet the discrete time api");

    const auto & yn = stencilDataManager.stateAt(ode::n());
    const auto & ynm1 = stencilDataManager.stateAt(ode::nMinusOne());
    const auto & ynm2 = stencilDataManager.stateAt(ode::nMinusTwo());
    system.template discreteTimeJacobian(step, t, dt, *J.data(),
					 *odeCurrentState.data(),
					 (*yn.data()),
					 (*ynm1.data()),
					 (*ynm2.data()) );
  }
};

}}}}//end namespace pressio::ode::implicitmethods::policy
#endif  // ODE_IMPLICIT_IMPL_STANDARD_POLICIES_ODE_IMPLICIT_DISCRETE_TIME_JACOBIAN_POLICY_HPP_
