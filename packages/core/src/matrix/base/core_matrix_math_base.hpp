
#ifndef CORE_MATRIX_BASE_MATRIX_MATH_BASE_HPP_
#define CORE_MATRIX_BASE_MATRIX_MATH_BASE_HPP_

#include "../core_matrix_traits.hpp"

namespace core{
    
template<typename derived_type>
class MatrixMathBase
  : private core::details::CrtpBase<MatrixMathBase<derived_type>>
{

private:
  using traits_t = details::traits<derived_type>;
  using der_t = typename traits_t::derived_t;
  using wrap_t = typename traits_t::wrapped_t;
  using ord_t = typename traits_t::ordinal_t;
  using sc_t = typename traits_t::scalar_t;    

public:
  void scale(sc_t factor){
    // this = factor * this
    this->underlying().scaleImpl(factor);
  };  
    
private:
  friend der_t;
  friend core::details::CrtpBase<MatrixMathBase<derived_type>>;

  MatrixMathBase() = default;
  ~MatrixMathBase() = default; 

};//end class

  
} // end namespace core
#endif
