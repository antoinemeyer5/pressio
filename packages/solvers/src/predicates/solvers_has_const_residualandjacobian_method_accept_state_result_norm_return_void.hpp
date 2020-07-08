
#ifndef solvers_has_const_residualandjacobian_method_accept_state_result_norm_return_void_HPP_
#define solvers_has_const_residualandjacobian_method_accept_state_result_norm_return_void_HPP_

namespace pressio{ namespace solvers{ namespace predicates {

template <
  typename T,
  typename state_t,
  typename res_t,
  typename jac_t,
  typename norm_t,
  typename = void
  >
struct has_const_residualandjacobian_method_accept_state_result_norm_return_void
  : std::false_type{};

template <
  typename T,
  typename state_t,
  typename res_t,
  typename jac_t,
  typename norm_t
  >
struct has_const_residualandjacobian_method_accept_state_result_norm_return_void<
  T, state_t, res_t, jac_t, norm_t, 
  mpl::enable_if_t<
    std::is_void<
      decltype(
         std::declval<T const>().residualAndJacobian
            (
              std::declval<state_t const &>(),
              std::declval<res_t &>(),
              std::declval<jac_t &>(),
              ::pressio::Norm::Undefined,
              std::declval<norm_t &>()
            )
         )
      >::value
    >
  > : std::true_type{};

}}} // namespace pressio::solvers::predicates
#endif
