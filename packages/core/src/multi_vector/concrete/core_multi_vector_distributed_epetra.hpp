
#ifndef CORE_MULTIVECTOR_CONCRETE_MULTIVECTOR_DISTRIBUTED_EPETRA_HPP_
#define CORE_MULTIVECTOR_CONCRETE_MULTIVECTOR_DISTRIBUTED_EPETRA_HPP_

#include "../../shared_base/core_container_base.hpp"
#include "../base/core_multi_vector_distributed_base.hpp"
#include "../base/core_multi_vector_distributed_trilinos.hpp"
#include "../base/core_multi_vector_math_base.hpp"
#include "../../shared_base/core_container_distributed_base.hpp"
#include "../../shared_base/core_operators_base.hpp"

namespace core{
  
template <typename wrapped_type>
class MultiVector<wrapped_type,
     typename
     std::enable_if<
       meta::is_multi_vector_epetra<
	    wrapped_type>::value
       >::type
     >
  : public ContainerBase< MultiVector<wrapped_type>, wrapped_type >,
    public MultiVectorDistributedBase< MultiVector<wrapped_type> >,
    public MultiVectorDistributedTrilinos< MultiVector<wrapped_type> >,
    public MultiVectorMathBase< MultiVector<wrapped_type> >,
    public Subscripting2DOperatorsBase< MultiVector<wrapped_type>,
      typename details::traits<MultiVector<wrapped_type>>::scalar_t,
      typename details::traits<MultiVector<wrapped_type>>::local_ordinal_t,
      typename details::traits<MultiVector<wrapped_type>>::global_ordinal_t>,
    public ContainerDistributedBase< MultiVector<wrapped_type>, 
      typename details::traits<MultiVector<wrapped_type>>::communicator_t >
{

private:
  using this_t = MultiVector<wrapped_type>;
  using sc_t = typename details::traits<this_t>::scalar_t;
  using LO_t = typename details::traits<this_t>::local_ordinal_t;
  using GO_t = typename details::traits<this_t>::global_ordinal_t;
  using wrap_t = typename details::traits<this_t>::wrapped_t;
  using map_t = typename details::traits<this_t>::data_map_t;
  using mpicomm_t = typename details::traits<this_t>::communicator_t;

public:
  MultiVector() = delete;

  explicit MultiVector(const map_t & mapobj, GO_t numVectors)
    : data_(mapobj, numVectors){}

  explicit MultiVector(const wrap_t & other) : data_(other){}

  ~MultiVector() = default;

public:
  sc_t & operator()(LO_t irow, GO_t icol){
    assert(icol < this->globalNumVectors() );
    assert(irow < this->localLength() );
    return data_[icol][irow];
  }

  sc_t const & operator()(LO_t irow, GO_t icol)const{
    assert(icol < this->globalNumVectors() );
    assert(irow < this->localLength() );
    return data_[icol][irow];
  }
  
private:
  wrap_t const * dataImpl() const{
    return &data_;
  }

  wrap_t * dataImpl(){
    return &data_;
  }

  bool emptyImpl() const{
    if (this->globalNumVectors()==0)
      return true;
    else
      return this->globalLength()==0  ? true : false;
  }

  void setZeroImpl() {
    data_.PutScalar(static_cast<sc_t>(0));
  }

  mpicomm_t const & commCRefImpl() const{
    return data_.Comm();
  }

  map_t const & getDataMapImpl() const{
    return data_.Map();
  }

  void replaceDataMapImpl(const map_t & mapObj){
    data_.ReplaceMap(mapObj);
  }

  GO_t globalNumVectorsImpl() const{
    return data_.NumVectors();
  }

  LO_t localNumVectorsImpl() const{
    // it is the same because epetra multivectors
    // are distributed on data, but each process owns
    // a part of each vector
    return data_.NumVectors();
  }
  
  GO_t globalLengthImpl() const {
    return data_.GlobalLength();
  };

  LO_t localLengthImpl() const {
    return data_.MyLength();
  };
    
  void replaceGlobalValueImpl(GO_t globalRowIndex,
			       GO_t vectorIndex,
			       sc_t value){
    data_.ReplaceGlobalValue(globalRowIndex, vectorIndex, value);
  }

  void scaleImpl(sc_t factor){
    data_.Scale(factor);
  }
  
private:
  friend ContainerBase< this_t, wrapped_type >;
  friend MultiVectorDistributedBase< this_t >;
  friend MultiVectorDistributedTrilinos< this_t >;
  friend MultiVectorMathBase< this_t >;
  friend ContainerDistributedBase< this_t, mpicomm_t >;
  friend Subscripting2DOperatorsBase< this_t, sc_t, LO_t, GO_t>;
  
private:
  wrap_t data_;

};//end class

}//end namespace core
#endif












//   sc_t & operator [] (LO_t i){
//     assert(i < this->localSize());
//     return data_[i];
//   };
//   sc_t const & operator [] (LO_t i) const{
//     assert(i < this->localSize());
//     return data_[i];
//   };  

//   der_t operator+(const der_t & other) const{
//     der_t res( other.getDataMap() );
//     for (LO_t i=0; i<this->localSize(); i++)
//       res[i] = (*this)[i] + other[i];
//     return res;
//   }

//   der_t operator-(const der_t & other) const{
//     der_t res( other.getDataMap() );
//     for (LO_t i=0; i<this->localSize(); i++)
//       res[i] = (*this)[i] - other[i];
//     return res;
//   }

//   der_t operator*(const der_t & other) const{
//     der_t res( other.getDataMap() );
//     for (LO_t i=0; i<this->localSize(); i++)
//       res[i] = (*this)[i] * other[i];
//     return res;
//   }

//   der_t & operator+=(const der_t & other) {
//     this->data_.Update(1.0, *other.data(), 1.0 );
//     return *this;
//   }

//   der_t & operator-=(const der_t & other) {
//     this->data_.Update(-1.0, *other.data(), 1.0 );
//     return *this;
//   }
    
// private:
//   //----------------
//   //from general base
//   //----------------
//   wrap_t const * dataImpl() const{
//     return &data_;
//   }

//   wrap_t * dataImpl(){
//     return &data_;
//   }

//   void putScalarImpl(sc_t value) {
//     data_.PutScalar(value);
//   }

//   void setZeroImpl() {
//     this->putScalarImpl( static_cast<sc_t>(0) );
//   }

//   bool emptyImpl() const{
//     return this->globalSize()==0 ? true : false;
//   }
  
//   //----------------
//   //from distributed base
//   //----------------
//   GO_t globalSizeImpl() const {
//     return data_.GlobalLength();
//   }

//   LO_t localSizeImpl() const {
//     return data_.MyLength();
//   }

//   void replaceGlobalValuesImpl(GO_t numentries,
//             const GO_t * indices,
//             const sc_t * values){
//     data_.ReplaceGlobalValues(numentries, values, indices);
//   }

//   //-------------------
//   //from trilinos base
//   //-------------------
//   map_t const & getDataMapImpl() const{
//     return data_.Map();
//   }

//   void replaceDataMapImpl(const map_t & mapObj){
//     data_.ReplaceMap(mapObj);
//   }
    
//   // //----------------
//   // //from math base
//   // //----------------
//   // template<typename op_t>
//   // void inPlaceOpImpl(op_t op, sc_t a1, sc_t a2, const der_t & other){
//   //   // this = a1*this op a2*other;
//   //   for (LO_t i=0; i<this->localSize(); i++)
//   //     data_[i] = op()( a1*data_[i], a2*other[i] );
//   // }

//   // void scaleImpl(sc_t & factor){
//   //   data_.Scale(factor);
//   // }

//   // void norm1Impl(sc_t & result) const {
//   //   data_.Norm1(&result);
//   // }

//   // void norm2Impl(sc_t & result) const {
//   //   data_.Norm2(&result);
//   // }

//   // void normInfImpl(sc_t & result) const {
//   //   data_.NormInf(&result);
//   // }

//   // void minValueImpl(sc_t & result) const {
//   //   data_.MinValue(&result);
//   // }

//   // void maxValueImpl(sc_t & result) const {
//   //   data_.MaxValue(&result);
//   // }
