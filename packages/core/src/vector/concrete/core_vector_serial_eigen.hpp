
#ifndef CORE_VECTOR_CONCRETE_VECTOR_SERIAL_EIGEN_HPP_
#define CORE_VECTOR_CONCRETE_VECTOR_SERIAL_EIGEN_HPP_

#include "../../shared_base/core_container_base.hpp"
#include "../../shared_base/core_operators_base.hpp"
#include "../base/core_vector_serial_base.hpp"
#include "../base/core_vector_math_base.hpp"


namespace core{
  
template <typename wrapped_type>
class Vector<wrapped_type,
	     typename std::enable_if<
	       core::meta::is_vector_eigen<wrapped_type>::value
	       >::type
	     >
  : public ContainerBase< Vector<wrapped_type>, wrapped_type >,
    public VectorSerialBase< Vector<wrapped_type> >,
    public VectorMathBase< Vector<wrapped_type> >,
    public ArithmeticOperatorsBase<Vector<wrapped_type>>,
    public CompoundAssignmentOperatorsBase<Vector<wrapped_type>>,
    public Subscripting1DOperatorsBase< Vector<wrapped_type>, 
              typename details::traits<Vector<wrapped_type>>::scalar_t,
              typename details::traits<Vector<wrapped_type>>::ordinal_t>
{

private:
  using this_t = Vector<wrapped_type>;
  using mytraits = typename details::traits<this_t>;  
  using sc_t = typename mytraits::scalar_t;
  using ord_t = typename  mytraits::ordinal_t;
  using wrap_t = typename mytraits::wrapped_t;

public:
  Vector() = default;

  explicit Vector(ord_t insize){
    this->resize(insize);
  }

  explicit Vector(const wrap_t & src) : data_(src){}

  Vector(this_t const & other)
    : data_(*other.data()){}
  
  ~Vector(){}
  
public:
  sc_t & operator [] (ord_t i){
    //assert(!this->empty());
    return data_(i);
  };

  sc_t const & operator [] (ord_t i) const{
    //assert(!this->empty());
    return data_(i);
  };  

  this_t operator+(const this_t & other) const{
    assert( other.size() == this->size() );
    this_t res(other.size());
    *res.data() = this->data_ + *other.data();
    return res;
  }

  this_t operator-(const this_t & other) const{
    assert( other.size() == this->size() );
    this_t res(other.size());
    *res.data() = this->data_ - *other.data();
    return res;
  }
  
  this_t operator*(const this_t & other) const{
    assert( other.size() == this->size() );
    this_t res(other.size());
    for (decltype(this->size()) i=0; i<this->size(); i++)
      res[i] = this->data_(i) * other[i];
    return res;
  }
  
  this_t & operator+=(const this_t & other) {
    assert( other.size() == this->size() );
    this->data_ += *other.data();
    return *this;
  }
  
  this_t & operator-=(const this_t & other) {
    assert( other.size() == this->size() );
    this->data_ -= *other.data();
    return *this;
  }

private:

  void matchLayoutWithImpl(const this_t & other){
    this.resize( other.size() );
  }
  
  wrap_t const * dataImpl() const{
    return &data_;
  }

  wrap_t * dataImpl(){
    return &data_;
  }

  void putScalarImpl(sc_t value) {
    data_.setConstant(value);
  }

  void setZeroImpl() {
    this->putScalarImpl( static_cast<sc_t>(0) );
  }

  bool emptyImpl() const{
    return this->size()==0 ? true : false;
  }

  //-----------------
  //from serial base
  //-----------------
  ord_t sizeImpl() const {
    return (data_.rows()==1) ? data_.cols() : data_.rows();
  }
  void resizeImpl(ord_t val){
    // check that the wrapped type is NOT a static vector from Eigen
    // otherwise we cannot resizee a static vector.
    static_assert(mytraits::isStatic == false,
		  "You cannot resize a vector wrapping a STATIC Eigen vector!");
    data_.resize(val);
  }

  //----------------
  //from math base
  //----------------
  template<typename op_t>
  void inPlaceOpImpl(sc_t a1, sc_t a2, const this_t & other){
    // this = a1*this op a2*other;
    for (decltype(this->size()) i=0; i<this->size(); i++)
      data_(i) = op_t()( a1*data_(i), a2*other[i] );
  }
  void scaleImpl(sc_t & factor){
    // this = factor * this;
    for (decltype(this->size()) i=0; i<this->size(); i++)
      data_(i) *= factor;
  }
  void norm1Impl(sc_t & result) const {
    result = static_cast<sc_t>(0);
    for (decltype(this->size()) i=0; i<this->size(); i++)
      result += std::abs(data_(i));
  }
  void norm2Impl(sc_t & res) const {
    res = static_cast<sc_t>(0);
    for (decltype(this->size()) i=0; i<this->size(); i++)
      res += data_(i)*data_(i);
    res = std::sqrt(res);
  }
  void normInfImpl(sc_t & res) const {
    res = std::abs(data_(0));
    for (decltype(this->size()) i=1; i<this->size(); i++){
      sc_t currVal = std::abs(data(i));
      if(currVal>res)
	res = currVal;
    }
  }
  void minValueImpl(sc_t & result) const {
    result = data_.minCoeff();
  }
  void maxValueImpl(sc_t & result) const {
    result = data_.maxCoeff();
  }
  
private:
  friend ContainerBase< this_t, wrapped_type >;
  friend VectorSerialBase< this_t >;
  friend VectorMathBase< this_t >;
  friend ArithmeticOperatorsBase< this_t >;
  friend CompoundAssignmentOperatorsBase< this_t >;  
  friend Subscripting1DOperatorsBase< this_t, sc_t, ord_t>;

private:
  wrap_t data_;
 
};//end class    
}//end namespace core
#endif
