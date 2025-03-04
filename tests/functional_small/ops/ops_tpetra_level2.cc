
#include "pressio/ops.hpp"
#include "tpetra_only_fixtures.hpp"
#include "ops_shared_level2.hpp"

//-------------------------------------------
// Test implementation and utilities
//-------------------------------------------

using vec_t = typename tpetraMultiVectorGlobSize15Fixture::vec_t;
using mvec_t = typename tpetraMultiVectorGlobSize15Fixture::mvec_t;
using importer_t = typename tpetraMultiVectorGlobSize15Fixture::importer_t;

// convenient alias for nicer test names
using ops_tpetra = tpetraMultiVectorGlobSize15Fixture;

template <typename T>
auto get_global_host_view(T &view, const importer_t& /* importer */) {
  return view;
}

template <>
auto get_global_host_view<mvec_t>(mvec_t &mv, const importer_t &importer) {
  auto mv_import = Teuchos::rcp(new mvec_t(importer.getTargetMap(), mv.getNumVectors()));
  mv_import->doImport(mv, importer, Tpetra::REPLACE);
  return mv_import->getLocalViewHost(Tpetra::Access::ReadWrite);
}

template <>
auto get_global_host_view<vec_t>(vec_t &v, const importer_t &importer) {
  auto v_import = Teuchos::rcp(new vec_t(importer.getTargetMap(), v.getNumVectors()));
  v_import->doImport(v, importer, Tpetra::REPLACE);
  auto v_h = v_import->getLocalViewHost(Tpetra::Access::ReadWrite);
  return Kokkos::subview(v_h, Kokkos::ALL(), 0);
}

template <
    typename FixtureType,
    typename TransMode,
    typename AType,
    typename XType,
    typename YType,
    typename ScalarType
    >
void test_impl(FixtureType &test, TransMode trans,
	       ScalarType alpha, AType A, XType x,
	       ScalarType beta, YType &y)
{
  // copy original values, fetch whole vector (from all ranks) to do the verification locally
  auto y_ref_h = get_global_host_view(y, *test.importer_);

  // call tested routine on device
  pressio::ops::product(trans, alpha, A, x, beta, y);

  // Note: trick here is to fetch whole data (from all ranks)
  // and run the simplified reference routine locally
  auto A_h = get_global_host_view(A, *test.importer_);
  auto x_h = get_global_host_view(x, *test.importer_);
  auto y_h = get_global_host_view(y, *test.importer_);
  if (test.rank_ == 0) {
    vanilla_gemv(trans, alpha, A_h, x_h, beta, y_ref_h);
    const size_t y_size = ::pressio::ops::extent(y_h, 0);
    for (size_t i = 0; i < y_size; ++i) {
      EXPECT_DOUBLE_EQ(y_h(i), y_ref_h(i));
    }
  }
}

template <
  typename FixtureType,
  typename TransMode,
  typename AType,
  typename XType,
  typename YType
  >
void test_impl(FixtureType &test, TransMode trans, AType A, XType x, YType y) {
  // non-zero alpha and beta
  test_impl(test, trans, 2., A, x, -1., y);
  // simulate beta=0 with uninitialized y containing NaN
  const auto nan = std::nan("0");
  ::pressio::ops::fill(y, nan);
  test_impl(test, trans, 2., A, x, 0., y);
  // simulate alpha=0 with NaN in input matrix
  auto A_h = A.getLocalViewHost(Tpetra::Access::ReadWrite);
  auto a00 = A_h(0, 0);
  A_h(0, 0) = nan;
  test_impl(test, trans, 0., A, x, 0., y);
  // alpha=0, beta=1
  ::pressio::ops::fill(y, 2.);
  test_impl(test, trans, 0., A, x, 1., y);
  // restore original value
  A_h(0, 0) = a00;
}

//-------------------------------------------
// Test Tuchos x
//-------------------------------------------

TEST_F(ops_tpetra, mv_prod_teuchos_vector)
{
  Teuchos::SerialDenseVector<int, double> x_teuchos(numVecs_);
  for (size_t i = 0; i < (size_t)numVecs_; ++i) {
    x_teuchos(i) = (double)(i + 1.);
  }
  // auto n = ::pressio::ops::extent(x_teuchos, 0);
  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_teuchos, *y_tpetra);
}

//-------------------------------------------
// Test Kokkos x
//-------------------------------------------

TEST_F(ops_tpetra, mv_prod_kokkos_vector)
{
  Kokkos::View<double*> x_kokkos{"x", (size_t)numVecs_};
  auto x_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), x_kokkos);
  for (int j = 0; j < numVecs_; ++j) {
    x_h(j) = (double)(j + 1.); // unique int values
  }
  Kokkos::deep_copy(x_kokkos, x_h);

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_kokkos, *y_tpetra);
}

TEST_F(ops_tpetra, mv_prod_kokkos_span)
{
  Kokkos::View<double*> x0{ "x_span", std::size_t(numVecs_ + 2) };
  auto x_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), x0);
  for (int i = 0; i < numVecs_ + 2; ++i) {
    x_h(i) = (double)(i + 1.); // unique int values
  }
  Kokkos::deep_copy(x0, x_h);
  auto x_kokkos_span = ::pressio::span(x0, 1, numVecs_);

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_kokkos_span, *y_tpetra);
  printf("@ [%d] DONE\n", rank_);
}

TEST_F(ops_tpetra, mv_prod_kokkos_diag)
{
  Kokkos::View<double**> x0{ "x_diag", std::size_t(numVecs_), std::size_t(numVecs_)};
  auto x_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), x0);
  for (size_t i = 0; i < (size_t)numVecs_; ++i) {
    for (size_t j = 0; j < (size_t)numVecs_; ++j) {
      x_h(i, j) = (double)(i * numVecs_ + j + 1.0);
    }
  }
  Kokkos::deep_copy(x0, x_h);
  auto x_kokkos_diag = ::pressio::diag(x0);

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_kokkos_diag, *y_tpetra);
}

//-------------------------------------------
// Test Kokkos y
//-------------------------------------------

TEST_F(ops_tpetra, mv_T_vector_storein_kokkos_vector)
{
  Kokkos::View<double*> y_kokkos{"y", (size_t)numVecs_};
  ::pressio::ops::fill(y_kokkos, 2.);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_kokkos);
}

TEST_F(ops_tpetra, mv_T_vector_storein_kokkos_span)
{
  Kokkos::View<double*> y0{ "x_span", std::size_t(numVecs_ + 2) };
  auto y_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), y0);
  for (int i = 0; i < numVecs_ + 2; ++i) {
    y_h(i) = (double)(i + 1.); // unique int values
  }
  Kokkos::deep_copy(y0, y_h);
  auto y_kokkos_span = ::pressio::span(y0, 1, numVecs_);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_kokkos_span);
}

TEST_F(ops_tpetra, mv_T_vector_storein_kokkos_diag)
{
  Kokkos::View<double**> y0{ "x_diag", std::size_t(numVecs_), std::size_t(numVecs_) };
  auto y_h = Kokkos::create_mirror_view(Kokkos::HostSpace(), y0);
  for (size_t i = 0; i < (size_t)numVecs_; ++i) {
    for (size_t j = 0; j <  (size_t)numVecs_; ++j) {
      y_h(i, j) = (double)(i * numVecs_ + j + 1.0);
    }
  }
  Kokkos::deep_copy(y0, y_h);
  auto y_kokkos_diag = ::pressio::diag(y0);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_kokkos_diag);
}

#ifdef PRESSIO_ENABLE_TPL_EIGEN

//-------------------------------------------
// Test Eigen x
//-------------------------------------------

TEST_F(ops_tpetra, mv_prod_eigen_vector)
{
  Eigen::VectorXd x_eigen{numVecs_};
  for (int j = 0; j < numVecs_; ++j) {
    x_eigen(j) = (double)(j + 1.);
  }

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_eigen, *y_tpetra);
}

TEST_F(ops_tpetra, mv_prod_eigen_span)
{
  Eigen::VectorXd x0(numVecs_ + 3);
  x0.setConstant(1.);
  auto x_eigen_span = pressio::span(x0, 2, numVecs_);

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_eigen_span, *y_tpetra);
}

TEST_F(ops_tpetra, mv_prod_eigen_diag)
{
  Eigen::MatrixXd M0(numVecs_, numVecs_);
  for (int i = 0; i < numVecs_; ++i) {
    M0(i, i) = 1.;
  }
  auto x_eigen_diag = pressio::diag(M0);

  test_impl(*this, ::pressio::nontranspose{}, *myMv_, x_eigen_diag, *y_tpetra);
}

//-------------------------------------------
// Test Eigen y
//-------------------------------------------

TEST_F(ops_tpetra, mv_T_vector_storein_eigen_vector)
{
  Eigen::VectorXd y_eigen{numVecs_};
  y_eigen.setConstant(1.);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_eigen);
}

TEST_F(ops_tpetra, mv_T_vector_storein_eigen_span)
{
  Eigen::VectorXd y0(numVecs_ + 3);
  y0.setConstant(1.);
  auto y_eigen_span = pressio::span(y0, 2, numVecs_);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_eigen_span);
}

TEST_F(ops_tpetra, mv_T_vector_storein_eigen_diag)
{
  Eigen::MatrixXd M0(numVecs_, numVecs_);
  for (int i = 0; i < numVecs_; ++i) {
    M0(i, i) = 1.;
  }
  auto y_eigen_diag = pressio::diag(M0);

  test_impl(*this, ::pressio::transpose{}, *myMv_, *x_tpetra, y_eigen_diag);
}
#endif
