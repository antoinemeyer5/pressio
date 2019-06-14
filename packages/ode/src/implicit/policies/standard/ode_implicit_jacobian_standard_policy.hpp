
#ifndef ODE_POLICIES_STANDARD_IMPLICIT_JACOBIAN_STANDARD_POLICY_HPP_
#define ODE_POLICIES_STANDARD_IMPLICIT_JACOBIAN_STANDARD_POLICY_HPP_

#include "../../../ode_forward_declarations.hpp"
#include "../base/ode_jacobian_policy_base.hpp"
#include "../../ode_jacobian_impl.hpp"

namespace rompp{ namespace ode{ namespace policy{

/*
 * state and jacobian types are core wrappers
 * both are wrappers from core
 */
template<typename state_type,
	 typename model_type,
	 typename jacobian_type>
class ImplicitJacobianStandardPolicy<
  state_type, model_type, jacobian_type,
  ::rompp::mpl::enable_if_t<
    ::rompp::ode::meta::is_legitimate_implicit_state_type<state_type>::value and
    ::rompp::ode::meta::is_legitimate_jacobian_type<jacobian_type>::value and
    core::meta::is_core_wrapper<state_type>::value and
    core::meta::is_core_wrapper<jacobian_type>::value
    >
  > : public JacobianPolicyBase<ImplicitJacobianStandardPolicy<
    state_type, model_type, jacobian_type> >{

  using this_t = ImplicitJacobianStandardPolicy<state_type, model_type, jacobian_type>;
  friend JacobianPolicyBase<this_t>;

public:
  ImplicitJacobianStandardPolicy() = default;
  ~ImplicitJacobianStandardPolicy() = default;

public:
  template <
    ode::ImplicitEnum method, typename scalar_t
  >
  void operator()(const state_type & y,
		  jacobian_type & J,
		  const model_type & model,
		  scalar_t t,
		  scalar_t dt)const
  {
    model.jacobian( *y.data(), *J.data(), t);
    ::rompp::ode::impl::time_discrete_jacobian<method>(J, dt);
  }

  template <
    ode::ImplicitEnum method, typename scalar_t
    >
  jacobian_type operator()(const state_type & y,
  			   const model_type & model,
  			   scalar_t t,
  			   scalar_t dt)const
  {
    auto nJJ = model.jacobian(*y.data(), t);
    jacobian_type JJ(nJJ);
    ::rompp::ode::impl::time_discrete_jacobian<method>(JJ, dt);
    return JJ;
  }

};//end class



#ifdef HAVE_PYBIND11
/*
 * state_type and jacobian_type = pybind11::array_t
 */
template<typename state_type,
	 typename model_type,
	 typename jacobian_type>
class ImplicitJacobianStandardPolicy<
  state_type, model_type, jacobian_type,
  ::rompp::mpl::enable_if_t<
    ::rompp::ode::meta::is_legitimate_implicit_state_type<state_type>::value and
    ::rompp::ode::meta::is_legitimate_jacobian_type<jacobian_type>::value and
    mpl::is_same<model_type, pybind11::object >::value and
    core::meta::is_array_pybind11<state_type>::value and
    core::meta::is_array_pybind11<jacobian_type>::value
    >
  > : public JacobianPolicyBase<ImplicitJacobianStandardPolicy<
    state_type, model_type, jacobian_type> >{

  using this_t = ImplicitJacobianStandardPolicy<state_type, model_type, jacobian_type>;
  friend JacobianPolicyBase<this_t>;

public:
  ImplicitJacobianStandardPolicy() = default;
  ~ImplicitJacobianStandardPolicy() = default;

public:
  template <
    ode::ImplicitEnum method, typename scalar_t
  >
  void operator()(const state_type & y,
		  jacobian_type & J,
		  const model_type & model,
		  scalar_t t,
		  scalar_t dt)const
  {
    throw std::runtime_error(" ImplicitJacobianStandardPolicy for pybind11 missing ");
    // model.jacobian( *y.data(), *J.data(), t);
    // ::rompp::ode::impl::time_discrete_jacobian<method>(J, dt);
  }

  template <
    ode::ImplicitEnum method, typename scalar_t
    >
  jacobian_type operator()(const state_type & y,
  			   const model_type & model,
  			   scalar_t t,
  			   scalar_t dt)const
  {
    auto nJJ = model.attr("jacobian1")(y, t);
    throw std::runtime_error(" ImplicitJacobianStandardPolicy for pybind11 missing ");

    // auto nJJ = model.jacobian(*y.data(), t);
    // jacobian_type JJ(nJJ);
    // ::rompp::ode::impl::time_discrete_jacobian<method>(JJ, dt);
    return nJJ;
  }
};//end class
#endif


}}}//end namespace rompp::ode::policy
#endif
