
#include "CORE_ALL"
#include "ODE_ALL"
#include "experimental/rom_galerkin_explicit_residual_policy.hpp"
#include "experimental/rom_matrix_pseudo_inverse.hpp"
#include "experimental/rom_pod.hpp"
#include "Epetra_MpiComm.h"

// template <typename T>
// void fillColloc(T & A, Epetra_Map map){
//   int myNR = map.NumMyElements();
//   std::vector<int> mygid(myNR);
//   map.MyGlobalElements( mygid.data() );
//   std::array<double,1> vals({1.});
//   std::array<int,1> colind;
//   for (auto const & it : mygid){
//     if(it == 0){
//       colind = {1};
//       A.insertGlobalValues(it, 1, vals.data(), colind.data());
//     }
//     if(it == 3){
//       colind = {5};
//       A.insertGlobalValues(it, 1, vals.data(), colind.data());
//     }
//   }
// }
using sc_t = double;
using dmat_t = core::Matrix<Epetra_MultiVector>;
using smat_t = core::Matrix<Epetra_CrsMatrix>;
using vd_t = std::vector<sc_t>;
using vvd_t = std::vector<vd_t>;

void readMatrixFromFile(std::string filename, vvd_t & A0, int ncols){
  
  std::ifstream source;
  source.open( filename, std::ios_base::in);
  std::string line, colv;
  vd_t tmpv(ncols);
  while (std::getline(source, line) ){
    std::istringstream in(line);
    for (int i=0; i<ncols; i++){
      in >> colv;
      tmpv[i] = atof(colv.c_str());
    }
    A0.emplace_back(tmpv);
  }
  source.close();

}


void doSVDHermitMat(int rank, Epetra_MpiComm & Comm)
{  
  // //every process reads the whole matrix
  vvd_t A0;
  int ncols = 6;
  readMatrixFromFile("mat_svd.dat", A0, ncols);
  int nrows = A0.size();
  assert(A0[0].size() == size_t(ncols));
  std::cout << A0.size() << " " << A0[0].size() << std::endl;

  // split matrix rows over processes
  Epetra_Map dataMap(nrows,0,Comm);

  // create a wrapper of a dense matrix
  dmat_t ADW(dataMap, ncols);
  // each process stores just its elements from A0
  int nMyElem = dataMap.NumMyElements();
  std::vector<int> myGel(nMyElem);
  dataMap.MyGlobalElements(myGel.data());
  for (int i=0; i<nMyElem; i++){
    int gi = myGel[i];
    for (int j=0; j<ncols; j++)
      ADW.replaceGlobalValue(gi, j, A0[gi][j]);
  }
  //ADW.data()->Print(std::cout);
  
  // /* AW is a DENSE matrix to do svd on
  //    To do this, we need to convert to sparse */
  auto ASW = core::denseToSparse(ADW);
  ADW.data()->Print(std::cout);

  int numVec = 6;
  auto U = rom::exp::pod(ASW, true, numVec);
  U.data()->Print(std::cout);

  std::vector<double> gold1 = {-0.951282108725694 ,  0.106996429673061,  0.087553772170267, 
			       0.223835908300265, -0.138478183372674,  0.08166841200593 };
  std::vector<double> gold2 = {-0.16732521814655  , -0.608982008204683,  0.530354433471666, 
			       -0.514683357113802,  0.223698195731366,  0.070196289707838};
  std::vector<double> gold3 = { 0.136796830814696 , -0.341533307993295,  0.410903537406805, 
				0.736326962199029,  0.01539855610482 , -0.39164448006505 };
  std::vector<double> gold4 = {-0.037274977813405 , -0.509335773946037, -0.383007418396895, 
			       -0.125796941462568, -0.721918123360113, -0.235589294204181};
  std::vector<double> gold5 = { 0.208194749722737 , -0.00929704388573 ,  0.333757208689705, 
				0.189877288948914, -0.465525121671036,  0.769680201801442};
  std::vector<double> gold6 = {-0.060139590009895 , -0.491465473461915, -0.533036807287154, 
			       0.301567368036207,  0.438937784524872,  0.432559156691345};
  
  double eps = 1e-10;
  if (rank == 0){
    for (int i=0; i<3; i++){
      assert( std::abs(U(i,0)) - std::abs(gold1[i]) < eps  );
      assert( std::abs(U(i,1)) - std::abs(gold2[i]) < eps  );
      assert( std::abs(U(i,2)) - std::abs(gold3[i]) < eps  );
      assert( std::abs(U(i,3)) - std::abs(gold4[i]) < eps  );
      assert( std::abs(U(i,4)) - std::abs(gold5[i]) < eps  );
      assert( std::abs(U(i,5)) - std::abs(gold6[i]) < eps  );
    }
  }
  if (rank == 1){
    for (int i=0; i<3; i++){
      assert( std::abs(U(i,0)) - std::abs(gold1[i+3]) < eps  );
      assert( std::abs(U(i,1)) - std::abs(gold2[i+3]) < eps  );
      assert( std::abs(U(i,2)) - std::abs(gold3[i+3]) < eps  );
      assert( std::abs(U(i,3)) - std::abs(gold4[i+3]) < eps  );
      assert( std::abs(U(i,4)) - std::abs(gold5[i+3]) < eps  );
      assert( std::abs(U(i,5)) - std::abs(gold6[i+3]) < eps  );
    }
  }
}




void doSVDNonSqMat(int rank, Epetra_MpiComm & Comm)
{  
  // //every process reads the whole matrix
  vvd_t A0;
  int ncols = 4;
  readMatrixFromFile("mat_svd_notsq.dat", A0, ncols);
  int nrows = A0.size();
  assert(A0[0].size() == size_t(ncols));
  std::cout << A0.size() << " " << A0[0].size() << std::endl;

  // split matrix rows over processes
  Epetra_Map dataMap(nrows,0,Comm);
  // create a wrapper of a dense matrix
  dmat_t ADW(dataMap, ncols);
  // each process stores just its elements from A0
  int nMyElem = dataMap.NumMyElements();
  std::vector<int> myGel(nMyElem);
  dataMap.MyGlobalElements(myGel.data());
  for (int i=0; i<nMyElem; i++){
    int gi = myGel[i];
    for (int j=0; j<ncols; j++)
      ADW.replaceGlobalValue(gi, j, A0[gi][j]);
  }
  //ADW.data()->Print(std::cout);
  
  // /* AW is a DENSE matrix to do svd on
  //    To do this, we need to convert to sparse */
  auto ASW = core::denseToSparse(ADW);
  ADW.data()->Print(std::cout);

  int numVec = 4;
  auto U = rom::exp::pod(ASW, false, numVec);
  U.data()->Print(std::cout);

  std::vector<double> gold1 = {-0.494496428275457, -0.737491969871685,  0.239944701905982,  
			      0.174403385546175, -0.233429626155463, -0.262867810063916};
  std::vector<double> gold2 = { 0.650040778401932, -0.520136386366104,  0.245861562608107,  
			       0.174635623369879,  0.15685195818428 ,  0.437443802434657};
  std::vector<double> gold3 = {-0.046872412374936,  0.30137401569448 ,  0.13482863890644 ,  
			      0.612677390475209, -0.633517866005814,  0.334782685342152};
  std::vector<double> gold4 = { 0.560224352296687, -0.055468255726513, -0.224800770072892,
			       0.135308116623717, -0.354258275935568, -0.699091802971507};
  
  double eps = 1e-10;
  if (rank == 0){
    for (int i=0; i<3; i++){
      assert( std::abs(U(i,0)) - std::abs(gold1[i]) < eps  );
      assert( std::abs(U(i,1)) - std::abs(gold2[i]) < eps  );
      assert( std::abs(U(i,2)) - std::abs(gold3[i]) < eps  );
      assert( std::abs(U(i,3)) - std::abs(gold4[i]) < eps  );
    }
  }
  if (rank == 1){
    for (int i=0; i<3; i++){
      assert( std::abs(U(i,0)) - std::abs(gold1[i+3]) < eps  );
      assert( std::abs(U(i,1)) - std::abs(gold2[i+3]) < eps  );
      assert( std::abs(U(i,2)) - std::abs(gold3[i+3]) < eps  );
      assert( std::abs(U(i,3)) - std::abs(gold4[i+3]) < eps  );
    }
  }
}





//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{  
  //-------------------------------
  // MPI init
  MPI_Init(&argc,&argv);
  int rank; // My process ID
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  Epetra_MpiComm Comm(MPI_COMM_WORLD);

  doSVDHermitMat(rank, Comm);
  doSVDNonSqMat(rank, Comm);
  
  
  //--------------------------------------------
  //--------------------------------------------
  // Epetra_Map dataMap(4,0,Comm);
  // int nMyElem = dataMap.NumMyElements();
  // std::vector<int> myGel(nMyElem);
  // dataMap.MyGlobalElements(myGel.data());

  // std::vector<double> Values(4);
  // std::vector<int> Indices(4);  
  // Epetra_CrsMatrix A(Epetra_DataAccess::Copy, dataMap, 4);
  // if (rank == 0){
  //   Values = {6.424838567610645,1.020098133467824,
  // 	      -0.140662180331129, -0.079765917033845};
  //   Indices = {0,1,2,3};
  //   A.InsertGlobalValues(0, 4, Values.data(), Indices.data());
     
  //   Values = {1.020098133467824,8.559661003361008,
  // 	      -2.682409559584041, -1.038273617013774};
  //   Indices = {0,1,2,3};
  //   A.InsertGlobalValues(1, 4, Values.data(), Indices.data());    
  // }
  // if (rank == 1){
  //   Values = {-0.140662180331129,-2.682409559584041,
  // 	      1.271948860180463, 1.249980167969988};
  //   Indices = {0,1,2,3};
  //   A.InsertGlobalValues(2, 4, Values.data(), Indices.data());
     
  //   Values = {-0.079765917033845, -1.038273617013774,
  // 	      1.249980167969988, 2.802877865356165};
  //   Indices = {0,1,2,3};
  //   A.InsertGlobalValues(3, 4, Values.data(), Indices.data());    
  // }
  // A.FillComplete();
  // core::Matrix<Epetra_CrsMatrix> ASW(A);
  // ASW.data()->Print(std::cout);
  
  //auto res = rom::exp::pod(ASW, 2, 2);
  //--------------------------------------------
  //--------------------------------------------
    
    
  MPI_Finalize();
  return 0;
}

