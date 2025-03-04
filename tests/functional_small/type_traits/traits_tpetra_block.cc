#include <gtest/gtest.h>
#include "pressio/type_traits.hpp"
#include "traits_shared.hpp"

namespace pressio { namespace traits { namespace test {

template <
  typename T,
  int rank,
  typename traits = pressio::Traits<T>
>
void test_tpetra_block_container()
{
  // traits and shared predicates
  test_container_traits<
    T,
    rank,
    typename T::impl_scalar_type
  >();

  // negative checks (cross-package)
  test_is_not_eigen_container<T>();
  test_is_not_kokkos_container<T>();
  test_is_not_teuchos_container<T>();
  test_is_not_epetra_container<T>();
}

TEST(tpetra_block, VectorTraits)
{
  using T = Tpetra::BlockVector<
    double,
    int, unsigned int,
    Tpetra::BlockVector<>::node_type
  >;

  // traits and shared predicates
  test_tpetra_block_container<T, 1>();

  // vector block
  static_assert(::pressio::is_vector_tpetra_block<T>::value,"");

  // negative checks (within Tpetra)
  static_assert(pressio::is_vector_tpetra<T>::value == false, "");
  static_assert(pressio::is_multi_vector_tpetra<T>::value == false, "");
  static_assert(pressio::is_multi_vector_tpetra_block<T>::value == false, "");
}

TEST(tpetra_block, MVTraits)
{
  using T = Tpetra::BlockMultiVector<
    double,
    int, unsigned int,
    Tpetra::MultiVector<>::node_type
  >;

  // traits and shared predicates
  test_tpetra_block_container<T, 2>();

  // multi-vector block
  static_assert(pressio::is_multi_vector_tpetra_block<T>::value,"");

  // negative checks (within Tpetra)
  static_assert(pressio::is_vector_tpetra<T>::value == false, "");
  static_assert(pressio::is_multi_vector_tpetra<T>::value == false, "");
  static_assert(pressio::is_vector_tpetra_block<T>::value == false, "");
}

}}} // pressio::traits::test