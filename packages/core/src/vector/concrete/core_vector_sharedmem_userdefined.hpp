
#ifndef CORE_VECTOR_CONCRETE_VECTOR_SHAREDMEM_USERDEFINED_HPP_
#define CORE_VECTOR_CONCRETE_VECTOR_SHAREDMEM_USERDEFINED_HPP_

#include "../../shared_base/core_container_base.hpp"
#include "../../shared_base/core_operators_base.hpp"
#include "../base/core_vector_sharedmem_base.hpp"
#include "../base/core_vector_math_base.hpp"

#include "../../meta/core_meta_detect_typedefs.hpp"
#include "../../meta/core_meta_detect_operators.hpp"

namespace core{
  
template <typename wrapped_type>
class Vector<wrapped_type,
	     typename std::enable_if<
	       !core::meta::is_vector_stdlib<wrapped_type>::value &&
	       !core::meta::is_vector_eigen<wrapped_type>::value &&
	       !core::meta::is_vector_epetra<wrapped_type>::value &&
	       !core::meta::is_vector_kokkos<wrapped_type>::value
	       >::type
	     >
  : public ContainerBase< Vector<wrapped_type>, wrapped_type>,
    public VectorSharedMemBase< Vector<wrapped_type> >,
    public VectorMathBase< Vector<wrapped_type> >,
    public ArithmeticOperatorsBase<Vector<wrapped_type>>,
    public CompoundAssignmentOperatorsBase<Vector<wrapped_type>>,
    public Subscripting1DOperatorsBase< Vector<wrapped_type>, 
              typename details::traits<Vector<wrapped_type>>::scalar_t,
              typename details::traits<Vector<wrapped_type>>::ordinal_t>    
{
private:
  using derived_t = Vector<wrapped_type>;
  using sc_t = typename details::traits<derived_t>::scalar_t;
  using der_t = typename details::traits<derived_t>::derived_t;
  using wrap_t = typename details::traits<derived_t>::wrapped_t;
  using ord_t = typename details::traits<derived_t>::ordinal_t;

public:
  Vector() = default;
  explicit Vector(ord_t insize,
		  sc_t value = static_cast<sc_t>(0) ){
    this->resize(insize, value);
  }
  explicit Vector(const wrap_t & obj) : data_(obj){};
  ~Vector(){};

public:
  sc_t & operator [] (ord_t i){
    return data_[i];
  };
  sc_t const & operator [] (ord_t i) const{
    return data_[i];
  };  

  //----------------------------------------------
  // on if native type has + operator
  template <typename U = derived_t>
  derived_t operator+(typename
		      std::enable_if<
		      meta::has_add_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    *res.data() = this->data_ + *other.data();
    return res;
  }

  // on if native type does NOT have + operator
  template <typename U = derived_t>
  derived_t operator+(typename
		      std::enable_if<
		      !meta::has_add_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    for (decltype(other.size()) i=0; i<other.size(); i++)
      res[i] = (*this)[i] + other[i];
    return res;
  }
  //----------------------------------------------

  // on if native type has - operator
  template <typename U = derived_t>
  derived_t operator-(typename
		      std::enable_if<
		      meta::has_subtract_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    *res.data() = this->data_ - *other.data();
    return res;
  }
  // on if native type does NOT have - operator
  template <typename U = derived_t>
  derived_t operator-(typename
		      std::enable_if<
		      !meta::has_subtract_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    for (decltype(other.size()) i=0; i<other.size(); i++)
      res[i] = (*this)[i] - other[i];
    return res;
  }
  //----------------------------------------------
  
  // on if native type has * operator
  template <typename U = derived_t>
  derived_t operator*(typename
		      std::enable_if<
		      meta::has_star_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    *res.data() = this->data_ * (*other.data());
    return res;
  }

  // on if native type does NOT have * operator
  template <typename U = derived_t>
  derived_t operator*(typename
		      std::enable_if<
		      !meta::has_star_op<wrap_t>::value,
		      const U & >::type other) const
  {
    derived_t res(other.size());
    for (decltype(other.size()) i=0; i<other.size(); i++)
      res[i] = (*this)[i] * other[i];
    return res;
  }
  //----------------------------------------------

  // on if native type has += operator
  template <typename U = derived_t>
  derived_t & operator+=(typename
		       std::enable_if<
		       meta::has_comp_assign_plus_op<wrap_t>::value,
		       const U & >::type other)
  {
    data_ += *other.data();
    return *this;
  }

  // on if native type does NOT have += operator
  template <typename U = derived_t>
  derived_t & operator+=(typename
		      std::enable_if<
		      !meta::has_comp_assign_plus_op<wrap_t>::value,
		      const U & >::type other)
  {
    for (decltype(other.size()) i=0; i<other.size(); i++)
      (*this)[i] += other[i];
    return *this;
  }
  //----------------------------------------------

  // on if native type has -= operator
  template <typename U = derived_t>
  derived_t & operator-=(typename
		       std::enable_if<
		       meta::has_comp_assign_minus_op<wrap_t>::value,
		       const U & >::type other)
  {
    data_ += *other.data();
    return *this;
  }

  // on if native type does NOT have -= operator
  template <typename U = derived_t>
  derived_t & operator-=(typename
		      std::enable_if<
		      !meta::has_comp_assign_minus_op<wrap_t>::value,
		      const U & >::type other)
  {
    for (decltype(other.size()) i=0; i<other.size(); i++)
      (*this)[i] -= other[i];
    return *this;
  }
  
private:
  // wrap_t const * dataImpl() const{
  //   return &data_;
  // };

  // wrap_t * dataImpl(){
  //   return data_;
  // };

  // void resizeImpl(ord_t val) {
  //   data_.resize(val);
  // };

  // ord_t sizeImpl() const {
  //   return data_.size();
  // };

  // TODO: missing everything here
  
private:
  friend ContainerBase< derived_t, wrapped_type >;
  friend VectorSharedMemBase< derived_t >;
  friend VectorMathBase< derived_t >;
  friend ArithmeticOperatorsBase< derived_t >;
  friend CompoundAssignmentOperatorsBase< derived_t >;  
  friend Subscripting1DOperatorsBase< derived_t, sc_t, ord_t>;

private:
   wrap_t data_;
  
};//end class
    
}//end namespace core
#endif

