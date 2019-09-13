/*
//@HEADER
// ************************************************************************
//
// solvers_linear_kokkos_direct.hpp
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

#ifdef HAVE_KOKKOS
#ifndef SOLVERS_LINEAR_KOKKOS_DIRECT_HPP
#define SOLVERS_LINEAR_KOKKOS_DIRECT_HPP

#include "../solvers_ConfigDefs.hpp"
#include "../base/solvers_linear_base.hpp"
#include "solvers_linear_traits.hpp"
#ifdef HAVE_TRILINOS
#include <Teuchos_LAPACK.hpp>
#include <Teuchos_SerialDenseSolver.hpp>
#endif

#if defined HAVE_KOKKOS and defined KOKKOS_ENABLE_CUDA
#include <cuda_runtime.h>
#include <cusolverDn.h>
#endif

namespace pressio { namespace solvers { namespace direct{

namespace impl{

template <typename T>
struct is_host_execution_space : std::false_type{};

#ifdef KOKKOS_ENABLE_SERIAL
template <>
struct is_host_execution_space<Kokkos::Serial> : std::true_type{};
#endif

#ifdef KOKKOS_ENABLE_OPENMP
template <typename T>
struct is_host_execution_space<Kokkos::OpenMP> : std::true_type{};
#endif

}


template<typename SolverT, typename MatrixT, typename enable = void>
class KokkosDirect;

/* GETRS */
template<typename SolverT, typename MatrixT>
class KokkosDirect<
  SolverT, MatrixT,
  mpl::enable_if_t<
    mpl::is_same<SolverT, ::pressio::solvers::linear::direct::getrs>::value
    >
  >
  : public LinearBase<SolverT, MatrixT, KokkosDirect<SolverT, MatrixT>>
{
public:
  static_assert( ::pressio::containers::meta::is_dense_matrix_wrapper_kokkos<MatrixT>::value or
  		 ::pressio::containers::meta::is_multi_vector_wrapper_kokkos<MatrixT>::value,
  		 "Kokkos direct dense solver expects either (a) dense matrix wrapper or a (b) multi-vector wrapper, both wrapping a rank=2 Kokkos View");

  using this_t          = KokkosDirect<SolverT, MatrixT>;
  using solver_t	= SolverT;
  using matrix_type	= MatrixT;
  using native_mat_t    = typename containers::details::traits<MatrixT>::wrapped_t;
  using scalar_t        = typename containers::details::traits<MatrixT>::scalar_t;
  using exe_space       = typename containers::details::traits<MatrixT>::execution_space;

  using base_t  = LinearBase<SolverT, MatrixT, this_t>;
  using solver_traits   = linear::details::traits<SolverT>;

  static_assert( solver_traits::kokkos_enabled == true,
  		 "the native solver must suppport kokkos to use in KokkosDirect");
  static_assert( solver_traits::direct == true,
  		 "the native solver must be direct to use in KokkosDirect");

public:
  KokkosDirect(){
#ifdef KOKKOS_ENABLE_CUDA
    auto cusolverStatus = cusolverDnCreate(&cuDnHandle_);
    assert(cusolverStatus == CUSOLVER_STATUS_SUCCESS);
#endif
  }

  KokkosDirect(const KokkosDirect &) = delete;

  ~KokkosDirect(){
#ifdef KOKKOS_ENABLE_CUDA
    auto cusolverStatus = cusolverDnDestroy(cuDnHandle_);
    assert(cusolverStatus == CUSOLVER_STATUS_SUCCESS);
#endif
  }

private:


// because this uses teuchos lapack wrapper
#ifdef HAVE_TRILINOS
  /*
   * enable if:
   * the matrix has layout left (i.e. column major)
   * T is a kokkos vector wrapper
   * has host execution space
   * T and MatrixT have same execution space
   */
  template <
    typename _MatrixT = MatrixT,
    typename _SolverT = SolverT,
    typename T,
    mpl::enable_if_t<
      mpl::is_same<
	typename ::pressio::containers::details::traits<_MatrixT>::layout,
	Kokkos::LayoutLeft
	>::value
      and
      ::pressio::containers::meta::is_vector_wrapper_kokkos<T>::value
      and
      ::pressio::solvers::direct::impl::is_host_execution_space<
	typename containers::details::traits<T>::execution_space
	>::value
      and
      mpl::is_same<
	typename containers::details::traits<T>::execution_space,
	typename containers::details::traits<_MatrixT>::execution_space
	>::value
      > * = nullptr
  >
  void solveAllowMatOverwriteImpl(_MatrixT & A, const T& b, T & y) {
    assert(A.rows() == b.size() );
    assert(A.cols() == y.size() );
    // gerts is for square matrices
    assert(A.rows() == A.cols() );

    // only one rhs because this is only enabled if T is a vector wrapper
    constexpr int nRhs = 1;

    // just use n, since rows == cols
    const auto n = A.rows();

    int info = 0;
    const int ipivSz = n;
    std::vector<int> ipiv(ipivSz);

    // LU factorize using GETRF
    lpk_.GETRF(n, n, A.data()->data(), n, ipiv.data(), &info);
    assert(info == 0);

    // we need to deep copy b into y and pass y
    // because getrs
    // overwrite the RHS in place with the solution
    Kokkos::deep_copy(*y.data(), *b.data());

    const char ct = 'N';
    lpk_.GETRS(ct, n, nRhs,
	       A.data()->data(),
	       n, ipiv.data(),
	       y.data()->data(),
	       y.size(),
	       &info);
    assert(info == 0);
  }
#endif


#if defined HAVE_KOKKOS and defined KOKKOS_ENABLE_CUDA
  /*
   * enable if:
   * the matrix has layout left (i.e. column major)
   * T is a kokkos vector wrapper
   * has CUDA execution space
   * T and MatrixT have same execution space
   */
  template <
    typename _MatrixT = MatrixT,
    typename _SolverT = SolverT,
    typename T,
    mpl::enable_if_t<
      mpl::is_same<
	typename ::pressio::containers::details::traits<_MatrixT>::layout,
	Kokkos::LayoutLeft
	>::value
      and
      ::pressio::containers::meta::is_vector_wrapper_kokkos<T>::value
      and
      mpl::is_same<
	typename containers::details::traits<T>::execution_space,
	Kokkos::Cuda
	>::value
      and
      mpl::is_same<
	typename containers::details::traits<T>::execution_space,
	typename containers::details::traits<_MatrixT>::execution_space
      >::value
      > * = nullptr
  >
  void solveAllowMatOverwriteImpl(_MatrixT & A, const T& b, T & y) {
    assert(A.rows() == b.size() );
    assert(A.cols() == y.size() );
    // gerts is for square matrices
    assert(A.rows() == A.cols() );

    // only one rhs because this is only enabled if T is a vector wrapper
    constexpr int nRhs = 1;

    // n = nRows = nCols: because it is square matrix
    const auto n = A.rows();

    cusolverStatus_t cusolverStatus;
    //cusolverDnHandle_t handle;
    cudaError cudaStatus;
    int Lwork = 0;

    // cuDnHandle already created in constructor

    // compute buffer size and prep.memory
    cusolverStatus = cusolverDnDgetrf_bufferSize(cuDnHandle_, n, n,
						 A.data()->data(),
						 n, &Lwork);
    assert(cusolverStatus == CUSOLVER_STATUS_SUCCESS);

    // for now, working buffers are stored as Kokkos arrays but
    // maybe later we can use directly cuda allocations
    using k1d_d = Kokkos::View<scalar_t*, Kokkos::LayoutLeft, exe_space>;
    using k1di_d = Kokkos::View<int*, Kokkos::LayoutLeft, exe_space>;
    ::pressio::containers::Vector<k1d_d> work_d("d_work", Lwork);
    ::pressio::containers::Vector<k1di_d> pivot_d("d_pivot", n);
    ::pressio::containers::Vector<k1di_d> info_d("d_info", 1);

    cusolverStatus = cusolverDnDgetrf(cuDnHandle_, n, n,
    				      A.data()->data(),
    				      n,
    				      work_d.data()->data(),
    				      pivot_d.data()->data(),
    				      info_d.data()->data());
    assert(cusolverStatus == CUSOLVER_STATUS_SUCCESS);

    // we need to deep copy b into y and pass y
    // because getrs
    // overwrite the RHS in place with the solution
    Kokkos::deep_copy(*y.data(), *b.data());

    cusolverStatus = cusolverDnDgetrs(cuDnHandle_, CUBLAS_OP_N,
    				      n, nRhs,
    				      A.data()->data(), n,
    				      pivot_d.data()->data(),
    				      y.data()->data(), n,
    				      info_d.data()->data());
    assert(cusolverStatus == CUSOLVER_STATUS_SUCCESS);

    // make sure the solver kernel is done before exiting
    cudaStatus = cudaDeviceSynchronize();
  }
#endif

  friend base_t;

#ifdef HAVE_TRILINOS
  Teuchos::LAPACK<int, scalar_t> lpk_;
#endif

#if defined HAVE_KOKKOS and defined KOKKOS_ENABLE_CUDA
  cusolverDnHandle_t cuDnHandle_;
#endif
};

}}} // end namespace pressio::solvers::direct
#endif
#endif
