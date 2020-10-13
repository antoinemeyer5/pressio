/*
//@HEADER
// ************************************************************************
//
// solvers_lm_hessian_gradient_operators.hpp
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

#ifndef SOLVERS_NONLINEAR_IMPL_OPERATORS_SOLVERS_LM_HESSIAN_GRADIENT_OPERATORS_HPP_
#define SOLVERS_NONLINEAR_IMPL_OPERATORS_SOLVERS_LM_HESSIAN_GRADIENT_OPERATORS_HPP_

namespace pressio{ namespace solvers{ namespace nonlinear{ namespace impl{

template <
  typename h_t, 
  typename g_t,
  typename r_t, 
  typename j_t,
  typename ud_ops_t,
  template<typename ...> class hgRJApi_t, 
  typename ...Args
  >
class LMHessianGradientOperatorsRJApi
{
  static constexpr auto pT  = ::pressio::transpose();
  static constexpr auto pnT = ::pressio::nontranspose();
  using sc_t = typename ::pressio::containers::details::traits<h_t>::scalar_t;

  // HGOpRJApi_ contains H = J^T J, and g = J^T r
  hgRJApi_t<h_t, g_t, r_t, j_t, ud_ops_t, Args...>  HGOpRJApi_;

  // lmH contains H + lm*diag(H)
  h_t lmH_;

  // damping factor for LM
  sc_t dampParam_ = pressio::utils::constants<sc_t>::one();

public:
  LMHessianGradientOperatorsRJApi() = delete;

  template <
   typename system_t, 
   typename state_t, 
   typename _ud_ops_t = ud_ops_t,
    mpl::enable_if_t<
      (pressio::solvers::concepts::system_residual_jacobian<system_t>::value or
       pressio::solvers::concepts::system_fused_residual_jacobian<system_t>::value)
      and std::is_void<_ud_ops_t>::value,
      int
     > = 0
  >
  LMHessianGradientOperatorsRJApi(const system_t & system, 
                                  const state_t & state)
    : HGOpRJApi_(system, state),
      lmH_(HGOpRJApi_.hessianCRef()){}

  template <
   typename system_t, 
   typename state_t, 
   typename ...ArgsIn,
    mpl::enable_if_t<
      (pressio::solvers::concepts::system_residual_jacobian<system_t>::value or
       pressio::solvers::concepts::system_fused_residual_jacobian<system_t>::value) 
      and sizeof ...(ArgsIn) >= 1,
      int
     > = 0
  >
  LMHessianGradientOperatorsRJApi(const system_t & system,
          const state_t & state,
          ArgsIn && ...args)
    : HGOpRJApi_(system, state, std::forward<ArgsIn>(args)...),
      lmH_(HGOpRJApi_.hessianCRef()){}


  // copy constr and assign
  LMHessianGradientOperatorsRJApi(LMHessianGradientOperatorsRJApi const &) = default;
  LMHessianGradientOperatorsRJApi & operator=(LMHessianGradientOperatorsRJApi const &) = default;

  // move constr and assign
  LMHessianGradientOperatorsRJApi(LMHessianGradientOperatorsRJApi && o) = default;
  LMHessianGradientOperatorsRJApi & operator=(LMHessianGradientOperatorsRJApi && o) = default;

  // destr
  ~LMHessianGradientOperatorsRJApi() = default;


public:
  h_t & hessianRef()     { return lmH_; }
  g_t & gradientRef()    { return HGOpRJApi_.gradientRef(); }
  const h_t & hessianCRef() const  { return lmH_; }
  const g_t & gradientCRef() const { return HGOpRJApi_.gradientCRef(); }

  const h_t & hessianCRefBeforeLMDiagonalScaling() const {
    return HGOpRJApi_.hessianCRef();
  }

  void setLMDampParam(sc_t parIn){ dampParam_ = parIn; }
  sc_t lmDampParam() const{ return dampParam_; }

public:
  void resetForNewCall(){
    dampParam_ = pressio::utils::constants<sc_t>::one();
  }

  template<typename system_t, typename state_t>
  void computeOperators(const system_t & sys,
      const state_t & state,
      sc_t & residualNorm,
      bool recomputeSystemJacobian=true)
  {
    HGOpRJApi_.computeOperators(sys, state,
        residualNorm,
        recomputeSystemJacobian);

    if(recomputeSystemJacobian){
      // compute lmH = H + mu*diag(H)
      const auto & H = HGOpRJApi_.hessianCRef();
      ::pressio::ops::deep_copy(lmH_, H);

      const auto diagH   = ::pressio::containers::diag(H);
      auto diaglmH = ::pressio::containers::diag(lmH_);
      constexpr auto one  = pressio::utils::constants<sc_t>::one();
      ::pressio::ops::do_update(diaglmH, one, diagH, dampParam_);
    }
  }

  template< typename system_t, typename state_t>
  void residualNorm(const system_t & system, 
         const state_t & state,
         sc_t & residualNorm) const
  {
    HGOpRJApi_.residualNorm(system, state, residualNorm);
  }
};



template <typename h_t, typename g_t>
class LMHessianGradientOperatorsHGApi
{
  using sc_t = typename ::pressio::containers::details::traits<h_t>::scalar_t;

  HessianGradientOperatorsHGApi<h_t, g_t> HGOpHGApi_;

  // lmH contains H + lm*diag(H)
  h_t lmH_;

  // damping factor for LM
  sc_t dampParam_ = pressio::utils::constants<sc_t>::one();

public:
  LMHessianGradientOperatorsHGApi() = delete;

  template <
   typename system_t, typename state_t,
    mpl::enable_if_t<
      pressio::solvers::concepts::system_hessian_gradient<system_t>::value or
      pressio::solvers::concepts::system_fused_hessian_gradient<system_t>::value,
      int
     > = 0
  >
  LMHessianGradientOperatorsHGApi(const system_t & system,
				  const state_t & state)
    : HGOpHGApi_(system, state),
      lmH_(HGOpHGApi_.hessianCRef())
  {}

  // copy constr and assign
  LMHessianGradientOperatorsHGApi(LMHessianGradientOperatorsHGApi const &) = default;
  LMHessianGradientOperatorsHGApi & operator=(LMHessianGradientOperatorsHGApi const &) = default;

  // move constr and assign
  LMHessianGradientOperatorsHGApi(LMHessianGradientOperatorsHGApi && o) = default;
  LMHessianGradientOperatorsHGApi & operator=(LMHessianGradientOperatorsHGApi && o) = default;

  // destr
  ~LMHessianGradientOperatorsHGApi() = default;

public:
  void resetForNewCall(){
    dampParam_ = pressio::utils::constants<sc_t>::one();
  }

  h_t & hessianRef()			{ return lmH_; }
  g_t & gradientRef()		{ return HGOpHGApi_.gradientRef(); }
  const h_t & hessianCRef() const	{ return lmH_; }
  const g_t & gradientCRef() const	{ return HGOpHGApi_.gradientCRef(); }

  const h_t & hessianCRefBeforeLMDiagonalScaling() const {
    return HGOpHGApi_.hessianCRef();
  }

  void setLMDampParam(sc_t parIn){ dampParam_ = parIn; }
  sc_t lmDampParam() const{ return dampParam_; }

  template< typename system_t, typename state_t>
  void residualNorm(const system_t & system, 
        const state_t & state,
        sc_t & residualNorm) const
  {
    system.residualNorm(state, ::pressio::Norm::L2, residualNorm);
  }

  template<typename system_t, typename state_t>
  mpl::enable_if_t<
    pressio::solvers::concepts::system_hessian_gradient<system_t>::value
    >
  computeOperators(const system_t & sys,
		   const state_t & state,
		   sc_t & residualNorm,
		   bool recomputeSystemJacobian = true)
  {
    HGOpHGApi_.computeOperators(sys, state,
				residualNorm, recomputeSystemJacobian);
    if (recomputeSystemJacobian){
      computeLMHessian();
    }
  }

  template<typename system_t, typename state_t>
  mpl::enable_if_t<
    pressio::solvers::concepts::system_fused_hessian_gradient<system_t>::value
    >
  computeOperators(const system_t & sys,
		   const state_t & state,
		   sc_t & residualNorm,
		   bool recomputeSystemJacobian = true)
  {
    HGOpHGApi_.computeOperators(sys, state,
				residualNorm, recomputeSystemJacobian);
    if (recomputeSystemJacobian){
      computeLMHessian();
    }
  }

private:
  void computeLMHessian()
  {
    // compute lmH = H + mu*diag(H)
    const auto & H = HGOpHGApi_.hessianCRef();
    ::pressio::ops::deep_copy(lmH_, H);

    const auto diagH   = ::pressio::containers::diag(H);
    auto diaglmH = ::pressio::containers::diag(lmH_);
    constexpr auto one  = pressio::utils::constants<sc_t>::one();
    ::pressio::ops::do_update(diaglmH, one, diagH, dampParam_);
  }
};

}}}}
#endif  // SOLVERS_NONLINEAR_IMPL_OPERATORS_SOLVERS_LM_HESSIAN_GRADIENT_OPERATORS_HPP_
