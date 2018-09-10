
#ifndef CORE_CONFIGDEFS_HPP_
#define CORE_CONFIGDEFS_HPP_

#include "core_crtp_helper.hpp"
#include "core_config.h"
#include <type_traits>
#include "core_shared_traits.hpp"

namespace core{
namespace details {

  
template<typename T, typename enable = void>
struct traits : public
containers_shared_traits<void, void,
			 false, false, false,
			 WrappedPackageIdentifier::Undefined,
			 false>
{};

template<typename T> 
struct traits<const T> : traits<T> {};
  

// //---------------------------------------
// // CRTP HELPER BASE CLASS
// //---------------------------------------
// template <typename T, typename enable = void>
// struct CrtpBase;
  
// template <typename T,
// 	  template<typename, typename...> class crtpType,
// 	  typename ... Args>
// struct CrtpBase< crtpType<T, Args...>>{
//   T & underlying() {
//     return static_cast<T&>(*this);
//   }
//   T const & underlying() const {
//     return static_cast<T const&>(*this);
//   }

// private:
//   CrtpBase() = default;
//   ~CrtpBase() = default;
//   friend crtpType<T, Args...>;
// };//end class


// template <typename T, int a, int b,
// 	  template<typename, int, int> class crtpType>
// struct CrtpBase< crtpType<T, a, b> >{
//   T & underlying() {
//     return static_cast<T&>(*this);
//   }
//   T const & underlying() const {
//     return static_cast<T const&>(*this);
//   }

// private:
//   CrtpBase() = default;
//   ~CrtpBase() = default;
//   friend crtpType<T, a, b>;
// };//end class

  
// template <typename T, int a,
// 	  template<typename, int> class crtpType>
// struct CrtpBase< crtpType<T, a> >{
//   T & underlying() {
//     return static_cast<T&>(*this);
//   }
//   T const & underlying() const {
//     return static_cast<T const&>(*this);
//   }

// private:
//   CrtpBase() = default;
//   ~CrtpBase() = default;
//   friend crtpType<T, a>;
// };//end class

  
//------------------------
} // end namespace details
//------------------------

  
   
namespace defaultTypes {

  //! Default value of Scalar template parameter.
  using scalar_t = double;

  //! Default value of LocalOrdinal template parameter.
  using local_ordinal_t = int;

  /// default global_ordinal_type
  using global_ordinal_t = int;

  // Unsigned int type
  using uint = unsigned int;

  // /// default type for error codes
  // using errcode_t = int;

  // admissible types for epetra vector
  using epetra_scalar_t = double;
  using epetra_lo_t = int;
  using epetra_go_t1 = int;
  using epetra_go_t2 = long long;

} // namespace defaultTypes


/// comment out for now because we do not do error checking
// constexpr defaultTypes::errcode_t _SUCCESS = 0;
// constexpr defaultTypes::errcode_t _FAILURE = 1;
  
} // end of core namespace

#endif
