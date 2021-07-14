/*
//@HEADER
// ************************************************************************
//
// solvers_base_updater.hpp
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

#ifndef SOLVERS_NONLINEAR_IMPL_UPDATERS_SOLVERS_BASE_UPDATER_HPP_
#define SOLVERS_NONLINEAR_IMPL_UPDATERS_SOLVERS_BASE_UPDATER_HPP_

namespace pressio { namespace solvers { namespace nonlinear { namespace impl {

struct BaseUpdater
{
  using apply_function_type = void (*)(BaseUpdater *, const void *, void *, void *);
  using reset_function_type = void (*)(BaseUpdater *);
  apply_function_type applyFnc_;
  reset_function_type resetFnc_;

  BaseUpdater() = default;
  BaseUpdater(BaseUpdater const &) = default;
  BaseUpdater & operator=(BaseUpdater const &) = default;
  BaseUpdater(BaseUpdater &&) = default;
  BaseUpdater & operator=(BaseUpdater &&) = default;
  virtual ~BaseUpdater() = default;

  template <class sys_t, class state_t, class solver_t>
  void operator()(const sys_t & S,
		  state_t & y,
		  solver_t & solver)
  {
    (*applyFnc_)(this, &S, &y, &solver);
  }

  void reset()
  {
    (*resetFnc_)(this);
  }
};

template <class sys_t, class state_t, class solver_t, class functor_t>
class Updater : public BaseUpdater
{
public:
  using functor_type = mpl::remove_cvref_t<functor_t>;
  using system_type = sys_t;
  using state_type = state_t;
  using solver_type = solver_t;

private:
  pressio::utils::instance_or_reference_wrapper<functor_t> F_;

public:
  template <class T>
  Updater(T && Fin)
    : F_(std::forward<T>(Fin)) {}

  Updater() = delete;
  Updater(Updater const &) = default;
  Updater & operator=(Updater const &) = default;
  Updater(Updater &&) = default;
  Updater & operator=(Updater &&) = default;
  ~Updater() = default;

  functor_type & get()
  {
    return F_.get();
  }
};

}}}}
#endif// SOLVERS_NONLINEAR_IMPL_UPDATERS_SOLVERS_BASE_UPDATER_HPP_
