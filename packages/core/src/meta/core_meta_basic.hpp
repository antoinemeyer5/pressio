
#ifndef CORE_META_META_BASIC_HPP_
#define CORE_META_META_BASIC_HPP_

#include <type_traits>
#include <complex>

namespace core{
namespace meta {

  template<typename T>
  struct remove_const: std::remove_const<T>{};

  template<typename T>
  struct remove_reference : std::remove_reference<T>{};

  template<typename T>
  struct remove_pointer : std::remove_pointer<T>{};

  template<typename T>
  struct is_arithmetic : std::is_arithmetic<T>{};

  template<typename T>
  struct is_integral: std::is_integral<T>{};

  /////////////////////////////////////////////////

    
  /*This allows allows for a shorter syntax:
      enable_if_t<a_condition, MyType>
    as opposed to:
      typename enable_if<a_certain_condition, MyType>::type
  */
  template<bool condition, typename T = void>
  using enable_if_t = typename std::enable_if<condition,T>::type;
  
  /////////////////////////////////////////////////

  
  template <typename... >
  using void_t = void;

  /////////////////////////////////////////////////

  
  // check if a type is default constructible
  // we leave this commented out, for now.
  // we use the std method instead.

  template<typename T>
  struct is_default_constructible
    : std::is_default_constructible<T> {};
  
  /////////////////////////////////////////////////  

  template <typename T,
	    typename enable = void>
  struct is_std_complex : std::false_type{};

  template <typename T>
  struct is_std_complex<T, typename
	  std::enable_if<
	   std::is_same<T,
	     std::complex<typename
		T::value_type
		>
	     >::value
	   >::type
	 > : std::true_type{};

  //////////////////////////////////////////////////

  template<typename T, typename base_t>
  struct publicly_inherits_from : std::is_base_of<base_t,T>{};
  
  //////////////////////////////////////////////////


  template<typename T,
	   typename = void>
  struct has_size_method : std::false_type{};

  template<typename T>
  struct has_size_method<T,
			typename
			std::enable_if<
			  !std::is_void<
			    decltype(std::declval<T>().size())
			    >::value
			  >::type
			> : std::true_type{};
  
   
} // namespace meta
} // namespace core
#endif
