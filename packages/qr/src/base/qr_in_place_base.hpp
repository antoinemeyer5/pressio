
#ifndef QR_BASE_IN_PLACE_BASE_HPP_
#define QR_BASE_IN_PLACE_BASE_HPP_

#include "../qr_meta.hpp"

namespace rompp{ namespace qr{

template<typename derived_t,
	 typename matrix_t>
class QRInPlaceBase
  : private core::details::CrtpBase<
  QRInPlaceBase<derived_t, matrix_t>>{

  using this_t = QRInPlaceBase<derived_t, matrix_t>;

  /* workaround for nvcc issue with templates, see https://devtalk.nvidia.com/default/topic/1037721/nvcc-compilation-error-with-template-parameter-as-a-friend-within-a-namespace/ */
  template<typename DummyType> struct dummy{using type = DummyType;};
  friend typename dummy<derived_t>::type;

  friend core::details::CrtpBase<this_t>;

public:
  void computeThin(matrix_t & A){
    this->underlying().computeThinImpl(A);
  }

private:
  QRInPlaceBase() = default;
  ~QRInPlaceBase() = default;

};

}}//end namespace rompp::qr
#endif
