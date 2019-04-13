
#ifndef CORE_VECTOR_BASE_VECTOR_DISTRIBUTED_BASE_HPP_
#define CORE_VECTOR_BASE_VECTOR_DISTRIBUTED_BASE_HPP_

#include "../core_vector_traits.hpp"

namespace rompp{ namespace core{

template<typename derived_type>
class VectorDistributedBase
  : private core::details::CrtpBase<
              VectorDistributedBase<derived_type>>
{
  static_assert( details::traits<derived_type>::is_shared_mem==0,
  "OOPS: non-distributed concrete vector inheriting from distributed base!");

  using this_t = VectorDistributedBase<derived_type>;
  using sc_t = typename details::traits<derived_type>::scalar_t;
  using LO_t = typename details::traits<derived_type>::local_ordinal_t;
  using GO_t = typename details::traits<derived_type>::global_ordinal_t;

public:
  template <typename T,
  	    ::rompp::mpl::enable_if_t<
	      std::is_same<T, sc_t>::value> * = nullptr>
  void putScalar(T value) {
    this->underlying().putScalarImpl(value);
  }

  GO_t globalSize() const {
    return this->underlying().globalSizeImpl();
  };

  LO_t localSize() const {
    return this->underlying().localSizeImpl();
  };

  void replaceGlobalValues(GO_t numentries,
			   const GO_t * indices,
			   const sc_t * values){
    this->underlying().replaceGlobalValuesImpl(numentries,
					       indices,
					       values);
  }

private:
  friend derived_type;
  friend core::details::CrtpBase<this_t>;

  VectorDistributedBase() = default;
  ~VectorDistributedBase() = default;

};//end class

}}//end namespace rompp::core
#endif
