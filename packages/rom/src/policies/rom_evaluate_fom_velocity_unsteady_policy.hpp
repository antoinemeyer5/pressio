/*
//@HEADER
// ************************************************************************
//
// rom_evaluate_fom_velocity_unsteady_policy.hpp
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

#ifndef ROM_EVALUATE_FOM_VELOCITY_UNSTEADY_HPP_
#define ROM_EVALUATE_FOM_VELOCITY_UNSTEADY_HPP_

namespace pressio{ namespace rom{ namespace policy{

template <>
struct EvaluateFomVelocityDefault<false>{

  //------------------------------------------
  // enabled for native c++
  //------------------------------------------
  template <
    typename fom_t,   typename state_t,
    typename rhs_t, typename time_t
#ifdef HAVE_PYBIND11
    , mpl::enable_if_t<
      mpl::not_same<fom_t, pybind11::object>::value and
      !::pressio::containers::meta::is_array_pybind11<state_t>::value and
      !::pressio::containers::meta::is_array_pybind11<rhs_t>::value
      > * = nullptr
#endif
    >
  void evaluate(const fom_t	& fomObj,
		const state_t & yFOM,
		rhs_t		& rhs,
		time_t		t) const{
    fomObj.velocity(*yFOM.data(), t, *rhs.data());
  }

  template <
    typename fom_t, typename state_t, typename time_t
#ifdef HAVE_PYBIND11
    , mpl::enable_if_t<
	mpl::not_same<fom_t, pybind11::object>::value and
	!::pressio::containers::meta::is_array_pybind11<state_t>::value
	> * = nullptr
#endif
    >
  auto evaluate(const fom_t	& fomObj,
		const state_t & yFOM,
		time_t		t) const
    -> decltype(fomObj.velocity(*yFOM.data(), t))
  {
    return fomObj.velocity(*yFOM.data(), t);
  }


  //------------------------------------------
  // enabled when interfacing with python
  //------------------------------------------
#ifdef HAVE_PYBIND11
  template <
    typename fom_t, typename state_t,
    typename rhs_t, typename time_t,
    mpl::enable_if_t<
      mpl::is_same<fom_t, pybind11::object>::value and
      ::pressio::containers::meta::is_array_pybind11<state_t>::value and
      ::pressio::containers::meta::is_array_pybind11<rhs_t>::value and
      // because we should have all = pybind11::array_t
      mpl::is_same<state_t, rhs_t>::value
      > * = nullptr
    >
  void evaluate(const fom_t	& fomObj,
		const state_t & yFOM,
		rhs_t		& rhs,
		time_t		t) const{
    fomObj.attr("velocity")(yFOM, t, rhs);
  }

  template <
    typename fom_t, typename state_t, typename time_t,
    mpl::enable_if_t<
      mpl::is_same<fom_t, pybind11::object>::value and
      ::pressio::containers::meta::is_array_pybind11<state_t>::value
      > * = nullptr
    >
  state_t evaluate(const fom_t	& fomObj,
		   const state_t & yFOM,
		   time_t t) const {
    return fomObj.attr("velocity")(yFOM, t);
  }
#endif
};

}}} //end namespace pressio::rom::policy
#endif
