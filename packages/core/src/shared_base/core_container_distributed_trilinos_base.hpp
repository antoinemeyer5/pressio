
#ifdef HAVE_TRILINOS
#ifndef CORE_SHARED_BASE_CONTAINER_DISTRIBUTED_TRILINOS_BASE_HPP_
#define CORE_SHARED_BASE_CONTAINER_DISTRIBUTED_TRILINOS_BASE_HPP_

#include "../core_ConfigDefs.hpp"
#include "Teuchos_RCPDecl.hpp"

namespace rompp{ namespace core{

template<typename derived_type, typename map_t>
class ContainerDistributedTrilinosBase
  : private core::details::CrtpBase<
  ContainerDistributedTrilinosBase<derived_type, map_t> >{

  using this_t = ContainerDistributedTrilinosBase<derived_type,map_t>;

public:
  map_t const & getDataMap() const{
    return this->underlying().getDataMapImpl();
  }

  bool hasRowMapEqualTo(map_t const &othermap) const{
    return this->underlying().hasRowMapEqualToImpl(othermap);
  }

  Teuchos::RCP<const map_t> getRCPDataMap() const{
    return this->underlying().getRCPDataMapImpl();
  }

  void replaceDataMap(const map_t & mapObj){
    return this->underlying().replaceDataMapImpl(mapObj);
  }

private:
  friend derived_type;
  friend core::details::CrtpBase<this_t>;

  ContainerDistributedTrilinosBase() = default;
  ~ContainerDistributedTrilinosBase() = default;

};//end class

}}//end namespace rompp::core
#endif
#endif
