
#ifndef ROM_DEFAULT_GALERKIN_EXPLICIT_RESIDUAL_POLICY_HPP_
#define ROM_DEFAULT_GALERKIN_EXPLICIT_RESIDUAL_POLICY_HPP_

#include "../rom_forward_declarations.hpp"
#include "../../../ode/src/explicit/policies/ode_explicit_residual_policy_base.hpp"
#include "../rom_data_fom_states.hpp"

namespace rompp{ namespace rom{

template <typename fom_states_data,
	  typename fom_rhs_data,
	  typename decoder_t>
class DefaultGalerkinExplicitResidualPolicy
  : public ode::policy::ExplicitResidualPolicyBase<
       DefaultGalerkinExplicitResidualPolicy<fom_states_data,
					     fom_rhs_data,
					     decoder_t>>,
    protected fom_states_data,
    protected fom_rhs_data{

protected:
  using this_t = DefaultGalerkinExplicitResidualPolicy<
		      fom_states_data, fom_rhs_data, decoder_t>;
  friend ode::policy::ImplicitResidualPolicyBase<this_t>;

  const decoder_t & decoder_;
  using fom_states_data::yFom_;
  using fom_rhs_data::fomRhs_;

public:
  static constexpr bool isResidualPolicy_ = true;

public:
  DefaultGalerkinExplicitResidualPolicy() = delete;
  ~DefaultGalerkinExplicitResidualPolicy() = default;
  DefaultGalerkinExplicitResidualPolicy(const fom_states_data & fomStates,
					const fom_rhs_data & fomResids,
					const decoder_t & decoder)
    : fom_states_data(fomStates),
      fom_rhs_data(fomResids),
      decoder_(decoder){}

public:
  template <typename galerkin_state_t,
	    typename galerkin_residual_t,
	    typename fom_t,
	    typename scalar_t>
  void operator()(const galerkin_state_t  & romY,
		  galerkin_residual_t	  & romR,
  		  const fom_t		  & app,
		  scalar_t		  t) const
  {
#ifdef HAVE_TEUCHOS_TIMERS
    auto timer = Teuchos::TimeMonitor::getStackedTimer();
    timer->start("galerkin explicit residual");
#endif

    fom_states_data::template reconstructCurrentFomState(romY);

#ifdef HAVE_TEUCHOS_TIMERS
    timer->start("fom eval rhs");
    app.residual(*yFom_.data(), *fomRhs_.data(), t);
    timer->stop("fom eval rhs");
#else
    app.residual(*yFom_.data(), *fomRhs_.data(), t);
#endif

#ifdef HAVE_TEUCHOS_TIMERS
    timer->start("phiT*fomRhs");
    core::ops::dot(decoder_.getJacobianRef(), fomRhs_, romR);
    timer->stop("phiT*fomRhs");
#else
    core::ops::dot(decoder_.getJacobianRef(), fomRhs_, romR);
#endif

#ifdef HAVE_TEUCHOS_TIMERS
    timer->stop("galerkin explicit residual");
#endif
  }

};//end class

}}//end namespace rompp::rom
#endif
