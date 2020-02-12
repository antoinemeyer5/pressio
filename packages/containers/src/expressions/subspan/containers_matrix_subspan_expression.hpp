/*
//@HEADER
// ************************************************************************
//
// containers_matrix_subspan_expressions.hpp
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

#ifndef CONTAINERS_MATRIX_SUBSPAN_EXPRESSION_HPP_
#define CONTAINERS_MATRIX_SUBSPAN_EXPRESSION_HPP_

#include "../../matrix/containers_matrix_meta.hpp"
#include "../containers_expression_base.hpp"

namespace pressio{ namespace containers{ namespace expressions{

template <typename matrix_t>
struct SubspanExpr<
  matrix_t,
  ::pressio::mpl::enable_if_t<
    ::pressio::containers::meta::is_matrix_wrapper_eigen<matrix_t>::value
    >
  >
  : public BaseExpr< SubspanExpr<matrix_t> >
{

  using interval_t = std::pair<std::size_t, std::size_t>;
  using native_t   = typename ::pressio::containers::details::traits<matrix_t>::wrapped_t;
  using scalar_t = typename ::pressio::containers::details::traits<matrix_t>::scalar_t;

private:
  matrix_t & matObj_;
  std::size_t rowStart_;
  std::size_t colStart_;
  std::size_t numRows_ = {};
  std::size_t numCols_ = {};

public:
  SubspanExpr() = delete;
  ~SubspanExpr() = default;
  SubspanExpr(const SubspanExpr & other) = default;
  SubspanExpr(SubspanExpr && other) = default;
  SubspanExpr & operator=(const SubspanExpr & other) = default;
  SubspanExpr & operator=(SubspanExpr && other) = default;

  SubspanExpr(matrix_t & matObjIn,
	      const interval_t rowRangeIn,
	      const interval_t colRangeIn)
    : matObj_(matObjIn)
  {
    rowStart_ = std::get<0>(rowRangeIn);
    colStart_ = std::get<0>(colRangeIn);

    assert( rowStart_ >= 0 and rowStart_ < matObjIn.extent(0) );
    assert( std::get<1>(rowRangeIn) <= matObjIn.extent(0) );
    assert( colStart_ >= 0 and colStart_ < matObjIn.extent(1) );
    assert( std::get<1>(colRangeIn) <= matObjIn.extent(1) );

    // here the ranges are exclusive of the last index (like Kokkos and Python)
    // so the indices of the last row and col included are:
    const auto endRow = std::get<1>(rowRangeIn)-1;
    const auto endCol = std::get<1>(colRangeIn)-1;
    assert(endRow >= rowStart_);
    assert(endCol >= colStart_);

    // # of rows and cols of the subspan
    numRows_ = endRow - rowStart_ + 1;
    numCols_ = endCol - colStart_ + 1;
  }

  std::size_t const & extent(std::size_t i) const{
    return (i==0) ? numRows_ : numCols_;
  }

  scalar_t & operator()(const std::size_t & i, const std::size_t & j)
  {
    assert(i < numRows_);
    assert(j < numCols_);
    return matObj_(rowStart_+i, colStart_+j);
  }

  scalar_t const & operator()(const std::size_t & i, const std::size_t & j) const
  {
    assert(i < numRows_);
    assert(j < numCols_);
    return matObj_(rowStart_+i, colStart_+j);
  }

  auto operator()()
    -> decltype(matObj_.data()->block(rowStart_, colStart_, numRows_, numCols_))
  {
    return matObj_.data()->block(rowStart_, colStart_, numRows_, numCols_);
  }

  auto operator()() const
    -> decltype( std::declval<const native_t>().block(rowStart_, colStart_, numRows_, numCols_))
  {
    return static_cast<native_t const *>(matObj_.data())->block(rowStart_, colStart_, numRows_, numCols_);
  }
};



#ifdef PRESSIO_ENABLE_TPL_KOKKOS
template <typename matrix_t>
struct SubspanExpr<
  matrix_t,
  ::pressio::mpl::enable_if_t<
    ::pressio::containers::meta::is_matrix_wrapper_kokkos<matrix_t>::value
    >
  >
  : public BaseExpr< SubspanExpr<matrix_t> >
{

  using interval_t = std::pair<std::size_t, std::size_t>;
  using native_t   = typename ::pressio::containers::details::traits<matrix_t>::wrapped_t;
  using scalar_t = typename ::pressio::containers::details::traits<matrix_t>::scalar_t;

private:
  matrix_t & matObj_;
  std::size_t rowStart_;
  std::size_t colStart_;
  std::size_t numRows_ = {};
  std::size_t numCols_ = {};
  using pair_t = std::pair<std::size_t, std::size_t>;

public:
  SubspanExpr() = delete;
  ~SubspanExpr() = default;
  SubspanExpr(const SubspanExpr & other) = default;
  SubspanExpr(SubspanExpr && other) = default;
  SubspanExpr & operator=(const SubspanExpr & other) = default;
  SubspanExpr & operator=(SubspanExpr && other) = default;

  SubspanExpr(matrix_t & matObjIn,
	      const interval_t rowRangeIn,
	      const interval_t colRangeIn)
    : matObj_(matObjIn)
  {
    rowStart_ = std::get<0>(rowRangeIn);
    colStart_ = std::get<0>(colRangeIn);

    assert( rowStart_ >= 0 and rowStart_ < matObjIn.extent(0) );
    assert( std::get<1>(rowRangeIn) <= matObjIn.extent(0) );
    assert( colStart_ >= 0 and colStart_ < matObjIn.extent(1) );
    assert( std::get<1>(colRangeIn) <= matObjIn.extent(1) );

    // here the ranges are exclusive of the last index (like Kokkos and Python)
    // so the indices of the last row and col included are:
    const auto endRow = std::get<1>(rowRangeIn)-1;
    const auto endCol = std::get<1>(colRangeIn)-1;
    assert(endRow >= rowStart_);
    assert(endCol >= colStart_);

    // # of rows and cols of the subspan
    numRows_ = endRow - rowStart_ + 1;
    numCols_ = endCol - colStart_ + 1;
  }

  std::size_t const & extent(std::size_t i) const{
    return (i==0) ? numRows_ : numCols_;
  }

  scalar_t & operator()(const std::size_t & i, const std::size_t & j)
  {
    assert(i < numRows_);
    assert(j < numCols_);
    return matObj_(rowStart_+i, colStart_+j);
  }

  scalar_t const & operator()(const std::size_t & i, const std::size_t & j) const
  {
    assert(i < numRows_);
    assert(j < numCols_);
    return matObj_(rowStart_+i, colStart_+j);
  }

  auto operator()()
    -> decltype
    (
     Kokkos::subview(*matObj_.data(), std::declval<pair_t>(), std::declval<pair_t>())
     )
  {
    return Kokkos::subview( *matObj_.data(),
			    std::make_pair(rowStart_, rowStart_+numRows_),
			    std::make_pair(colStart_, colStart_+numCols_));
  }

  auto operator()() const
    -> decltype
    (
     Kokkos::subview(*matObj_.data(), std::declval<pair_t>(), std::declval<pair_t>())
     )
  {
    return Kokkos::subview( *matObj_.data(),
			    std::make_pair(rowStart_, rowStart_+numRows_),
			    std::make_pair(colStart_, colStart_+numCols_));
  }
};
#endif


}}} //end namespace pressio::containers::expressions
#endif
