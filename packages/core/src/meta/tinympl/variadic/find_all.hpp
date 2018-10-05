/*
//@HEADER
// ************************************************************************
//
//                          find_all.hpp
//                         darma_new
//              Copyright (C) 2017 NTESS, LLC
//
// Under the terms of Contract DE-NA-0003525 with NTESS, LLC,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact somebody@sandia.gov
//
// ************************************************************************
//@HEADER
*/

#ifndef SRC_META_TINYMPL_VARIADIC_FIND_ALL_HPP_
#define SRC_META_TINYMPL_VARIADIC_FIND_ALL_HPP_

#include <type_traits>

#include "find_all_if.hpp"
#include "../lambda.hpp"
#include "../bind.hpp"
#include "../equal_to.hpp"

namespace tinympl {
namespace variadic {

template <typename T, typename... Args>
struct find_all
  : public find_all_if<
      lambda<equal_to<tinympl::placeholders::_, T>>::template apply,
      Args...
    >
{ };


} // end namespace variadic
} // end namespace tinympl



#endif /* SRC_META_TINYMPL_VARIADIC_FIND_ALL_HPP_ */
