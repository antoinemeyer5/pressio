
#ifndef ODE_IS_LEGITIMATE_MODEL_FOR_EXPLICIT_ODE_HPP_
#define ODE_IS_LEGITIMATE_MODEL_FOR_EXPLICIT_ODE_HPP_

#include "../../../core/src/meta/core_meta_has_scalar_typedef.hpp"
#include "ode_has_state_typedef.hpp"
#include "ode_has_residual_typedef.hpp"
#include "ode_model_has_all_needed_residual_methods.hpp"

namespace rompp{ namespace ode{ namespace meta {

template<typename model_type,
	 typename enable = void>
struct is_legitimate_model_for_explicit_ode : std::false_type{};

template<typename model_type>
struct is_legitimate_model_for_explicit_ode<
  model_type,
  typename std::enable_if<
    ::rompp::core::meta::has_scalar_typedef<model_type>::value and
    has_state_typedef<model_type>::value and
    has_residual_typedef<model_type>::value and
    model_has_needed_residual_methods<
      model_type,
      typename model_type::state_type,
      typename model_type::residual_type,
      typename model_type::scalar_type>::value
    >::type
  > : std::true_type{};

//-------------------------------------------------------

}}} // namespace rompp::ode::meta
#endif
