
#ifndef CORE_CRPT_HELPER_HPP_
#define CORE_CRPT_HELPER_HPP_ 

namespace core{
  
template <typename T,
	  template<typename T, typename...> class crtpType,
	  typename ... Args>
struct crtpBase{
  // T * underlying(){
  //   return static_cast< T* >( this );
  // }
  // const T * underlying() const{
  //   return static_cast< const T* >( this );
  // }
  T & underlying() {
    return static_cast<T&>(*this);
  }
  T const & underlying() const {
    return static_cast<T const&>(*this);
  }
private:
  crtpBase(){}
  friend crtpType<T, Args...>;

};//end class
}//end namespace
#endif
