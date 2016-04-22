


#include "FemusInit.hpp"
#include "MultiLevelProblem.hpp"
#include "VTKWriter.hpp"
#include "adept.h"

#include "MeshRefinement.hpp"


using namespace femus;

bool SetBoundaryCondition(const std::vector < double >& x, const char SolName[], double& value, const int facename, const double time) {
  bool dirichlet = false; //dirichlet
  value = 0.;
  return dirichlet;
}

double InitalValueW(const std::vector < double >& x) {
  return 1+x[0]-x[1];
}


double InitalValueV(const std::vector < double >& x) {
  return 1+x[0]-x[1];
}

int main(int argc, char** args) {

  // init Petsc-MPI communicator
  FemusInit mpinit(argc, args, MPI_COMM_WORLD);

  const elem_type * _finiteElement1 = new const elem_type_2D("quad","disc_linear","seventh");
  std::cout<<std::endl;
  const elem_type * _finiteElement2 = new const elem_type_2D("tri","disc_linear","seventh");
  
  std::cout<<std::endl;
  const elem_type * _finiteElement3 = new const elem_type_3D("hex","disc_linear","seventh");
  
  //const elem_type * _finiteElement1 = new const elem_type_2D("tri","quadratic","seventh");
  //const elem_type * _finiteElement2 = new const elem_type_2D("tri","biquadratic","seventh");
  
  //const elem_type * _finiteElement3 = new const elem_type_3D("wedge","quadratic","seventh");
  //const elem_type * _finiteElement4 = new const elem_type_3D("wedge","biquadratic","third");
  
  //const elem_type * _finiteElement5 = new const elem_type_3D("tet","quadratic","seventh");
  //const elem_type * _finiteElement6 = new const elem_type_3D("tet","biquadratic","seventh");
  
  /*
  
  
  MultiLevelMesh mlMsh;
  // read coarse level mesh and generate finers level meshes
  double scalingFactor = 1.;
  //mlMsh.ReadCoarseMesh("./input/triangle.neu", "seventh", scalingFactor);
  //mlMsh.ReadCoarseMesh("./input/wedge1.neu", "seventh", scalingFactor);
  mlMsh.ReadCoarseMesh("./input/tet2.neu", "seventh", scalingFactor);
  //mlMsh.ReadCoarseMesh("./input/cube_hex.neu", "seventh", scalingFactor);
  //mlMsh.ReadCoarseMesh("./input/square_quad.neu", "seventh", scalingFactor);
  
  
  unsigned dim = mlMsh.GetDimension();
  unsigned maxNumberOfMeshes = 5;
  //vector < vector < double > > l2Norm;
  //l2Norm.resize (maxNumberOfMeshes);
  //vector < vector < double > > semiNorm;
  //semiNorm.resize (maxNumberOfMeshes);
  unsigned numberOfUniformLevels = 4;
  unsigned numberOfSelectiveLevels = 0;
  mlMsh.RefineMesh(numberOfUniformLevels , numberOfUniformLevels + numberOfSelectiveLevels, NULL);
  
  mlMsh.PrintInfo();

  MultiLevelSolution mlSol(&mlMsh);

  // add variables to mlSol

  mlSol.AddSolution("V", LAGRANGE, SECOND);
  mlSol.AddSolution("W", DISCONTINOUS_POLYNOMIAL, FIRST);
  
  mlSol.Initialize("All");
  
  mlSol.Initialize("V",InitalValueV);
  mlSol.Initialize("W",InitalValueW);
  
  mlSol.AttachSetBoundaryConditionFunction(SetBoundaryCondition);

  mlSol.GenerateBdc("All");
  
  
  MeshRefinement meshcoarser(*mlMsh.GetLevel(numberOfUniformLevels-1));
  meshcoarser.FlagAllElementsToBeRefined();
  mlMsh.AddAMRMeshLevel();
  mlSol.AddSolutionLevel();
  mlSol.RefineSolution(numberOfUniformLevels);
  
  
  
  std::vector < std::string > variablesToBePrinted;
  variablesToBePrinted.push_back("All");

  VTKWriter vtkIO(&mlSol);
  vtkIO.Write(DEFAULT_OUTPUTDIR, "biquadratic", variablesToBePrinted);*/
  
  
  return 0;
}

