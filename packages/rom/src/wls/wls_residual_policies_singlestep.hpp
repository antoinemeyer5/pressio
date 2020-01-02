namespace pressio{ namespace rom{ namespace wls{ namespace impl{
/* 
This header file contains policies for computing the time local residuals; i.e., 
the residual at a single time step,
*/
//============ For the velocity APIs ===========================
//template< typename fom_type>
struct local_residual_policy_velocityAPI{
public:
  //BDF2
  template <typename fom_type, 
						typename fom_state_t, 
						typename residual_t, 
						typename aux_states_container_t, 
						typename scalar_t>
  void operator()(const ::pressio::ode::implicitmethods::BDF2  & tag, 
									const fom_type & appObj,
									const fom_state_t & yFOM, 
									residual_t & residual,
									const aux_states_container_t & auxStatesContainer,
									const scalar_t & t,
									const  scalar_t & dt, 
									const int & step) const{
    if (step > 0){
    // u^n - 4./3.*u^{n-1} + 1./3.u^{n-2} - 2./3.*dt*f
    appObj.velocity(*yFOM.data(),t,*residual.data());
    ::pressio::ode::impl::time_discrete_residual<::pressio::ode::implicitmethods::BDF2>(yFOM,residual,auxStatesContainer,dt);
    }
    if (step == 0){
    // u^n - u^{n-1}  - dt*f
    appObj.velocity(*yFOM.data(),t,*residual.data());
    ::pressio::ode::impl::time_discrete_residual<::pressio::ode::implicitmethods::Euler>(yFOM,residual,auxStatesContainer,dt);
    } 
  }
  // Implicit Euler
  template <typename fom_type,
						typename fom_state_t, 
						typename residual_t, 
						typename aux_states_container_t, 
						typename scalar_t>
  void operator()(const ::pressio::ode::implicitmethods::Euler & tag, 
									const fom_type & appObj,
									const fom_state_t & yFOM, 
									residual_t & residual,
									const aux_states_container_t & auxStatesContainer,
									const scalar_t & t,
									const scalar_t & dt, 
									const int & step) const{
    appObj.velocity(*yFOM.data(),t,*residual.data());
    ::pressio::ode::impl::time_discrete_residual<::pressio::ode::implicitmethods::Euler>(yFOM,residual,auxStatesContainer,dt);
  } 
};

//============================
}}}}
