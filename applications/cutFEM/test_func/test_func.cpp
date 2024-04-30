#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <algorithm>    // std::sort
#include <ctime>
#include <cstdlib>
#include <climits>
#include <typeinfo>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <fstream>
#include <string>
// #include <filesystem>

namespace fs = boost::filesystem;

using namespace std;

#include "Rebuild.hpp"
#include "parabolaIntegration.hpp"
#include "PolynomialBases.hpp"



int main() {
  typedef cpp_bin_float_oct Type;
  int s = 0;
  int table = 0;
  Type a(0);
  Type c(1);
  PointT <Type> p1, p2, p3;
  p1 = { static_cast<Type>(0), static_cast<Type>(0.11) };
  p2 = { static_cast<Type>(0.33), static_cast<Type>(1) };
  p3 = { static_cast<Type>((p1.x + p2.x) / 2.0), static_cast<Type>(0.55) };
  std::vector<double>weightCF;

  CutFemWeightParabola <double, Type> Pweights(QUAD, 3, "legendre");
  Pweights(s, a, c, table, p1, p2, p3, weightCF);

  double Area0 = 0;
  double Area = 0;
  double Ix = 0;
  double Iy = 0;

  double Ix3 = 0;
  double Ix2y = 0;
  double Ixy2 = 0;
  double Iy3 = 0;

  double Ix2y2 = 0;

  const double* gaussWeight =  Pweights.GetGaussWeightPointer();
  const double* xg = Pweights.GetGaussCoordinatePointer(0);
  const double* yg = Pweights.GetGaussCoordinatePointer(1);

//     cout << " gaussWeight = " << *gaussWeight <<endl;
//     cout<< "gaussWeight = " << weightCF.size() << endl;
//     for(unsigned ig = 0; ig < weightCF.size(); ig++) {
//     cout  << gaussWeight[ig] << " ";
//     }
//     cout<<endl;

  cout << " weight CF = " << weightCF.size() << endl;
  for(unsigned ig = 0; ig < weightCF.size(); ig++) {
    cout  << weightCF[ig] << " ";
  }
  cout << endl;

  for(unsigned ig = 0; ig < weightCF.size(); ig++) {

    Area0 += gaussWeight[ig];
    Area += weightCF[ig] * gaussWeight[ig];
    Ix += xg[ig] * weightCF[ig] * gaussWeight[ig];
    Iy += yg[ig] * weightCF[ig] * gaussWeight[ig];


    Ix3 += xg[ig] * xg[ig] * xg[ig] * weightCF[ig] * gaussWeight[ig];
    Ix2y += xg[ig] * xg[ig] * yg[ig] * weightCF[ig] * gaussWeight[ig];
    Ixy2 += xg[ig] * yg[ig] * yg[ig] * weightCF[ig] * gaussWeight[ig];
    Iy3 += yg[ig] * yg[ig] * yg[ig] * weightCF[ig] * gaussWeight[ig];

    Ix2y2 += xg[ig] * xg[ig] * yg[ig] * yg[ig] * weightCF[ig] * gaussWeight[ig];


  }

  std::cout << "Area0 = " << Area0 << std::endl;
  std::cout << "Area = " << Area << std::endl;
  std::cout << "Ix = " << Ix << std::endl;
  std::cout << "Iy = " << Iy << std::endl;

  std::cout << "Ix3 = " << Ix3 << std::endl;
  std::cout << "Ix2y = " << Ix2y << std::endl;
  std::cout << "Ixy2 = " << Ixy2 << std::endl;
  std::cout << "Iy3 = " << Iy3 << std::endl;

  std::cout << "Ix2y2 = " << Ix2y2 << std::endl;

  unsigned nInt;

  //std::vector<std::vector<double>> xv = {{0., 1., 2., 1.}, {0., 0., 1., 1.}};
  std::vector<std::vector<double>> xv = {{1., 1., 0., 0.}, { 0., 1., 1., 0.}};
  std::vector<double> A = {1, 0, 1, 0, 0, -0.5};
  unsigned nPoints = 3;
  unsigned dim = 2;
  short unsigned ielType = 3; //quad
  unsigned femType = 0; //linear FEM

  std::pair<std::vector<std::vector<double>>, std::vector<double>> xp = GetCellPointsFromQuadric(xv, A, nPoints, nInt);



  std::vector < std::vector < std::vector <double > > > aP(1);
  ProjectNodalToPolynomialCoefficients(aP[femType], xv, ielType, femType);


  std::vector<std::vector<double>> xi(nPoints, std::vector<double>(2, 0.));
  for(unsigned i = 0; i < nPoints; i++) {
    bool inverseMapping = GetInverseMapping(femType, ielType, aP, xp.first[i], xi[i], 100);
    xi[i] = {0.5 * (xi[i][0] + 1.), 0.5 * (xi[i][1] + 1.)};
    std::cout << i << " " << xi[i][0] << " " << xi[i][1] << std::endl;
  }

  // std::vector<double> A = {1, 0, 1, 0, 0, -1};
  // std::vector<double> P1 = {1, 0.5};
  // std::vector<double> P2 = {-.5, -1};
  // std::vector<std::vector<double>> P;
  // BuildMarkersOnConicArc(M_PI, 5, A, {2, 2}, P1, P2, P);


//   return 0;
// //     std::cout.precision(16);
//     srand(10); // Fixed seed for random number generation

  int maxDepth = 3;
  std::vector<OctreeNode<Type>> roots;

  for(int ttable = 0; ttable < 1; ++ttable) {
    std::string filename = "save/octree_data" + std::to_string(ttable) + ".csv";

//     if (fs::exists(filename)) {
//       std::cout << "File " << filename << " already exists. Skipping octree generation." << std::endl;
//     }
//
//     else{
      OctreeNode<Type> root({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, ttable, 0, 3, &Pweights);
      if(ttable == 0 || ttable == 1 || ttable == 2 || ttable == 4 || ttable == 6) {
        root.subdivideWithRelativeError(maxDepth, 0.001);
      }
      else {
        root.subdivideWithRelativeError(3, 0.1);
      }

      root.saveOctreeToCSV(filename);
      std::cout << "Octree Structure:\n";
      roots.push_back(root);
//     }
  }

    // Load the octree structure and vectors from the CSV file
    std::vector<OctreeNode<Type>>loadedRoots;

    for(int ttable = 0; ttable < 1; ++ttable) {
      OctreeNode<Type> loadedRoot({0., 0., 0.}, {1., 1., 1.}, 0, 0, 0, nullptr);
      loadedRoot.loadOctreeFromCSV("octree_data.csv");
      loadedRoots.push_back(loadedRoot);
    }


    // Example: Search for a point in the loaded Octree
    table = 0 ;
    Point3D searchPoint(0.11, 0.33, 0.55);
    OctreeNode<Type>* result = loadedRoots[table].search(searchPoint);
    if (result) {
        std::cout << "Found the smallest sub-cube containing the search point." << std::endl;
    } else {
        std::cout << "Search point not found in the Octree." << std::endl;
    }

    // Print the loaded Octree structure
//     printOctreeStructure(&loadedRoot);







//   return 0;


//   std::vector<OctreeNode<Type>> roots;
//
//   for(int ttable = 0; ttable < 1; ++ttable) {
//     OctreeNode<Type> root({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, ttable, 0, 3, &Pweights);
//     if(ttable == 0 || ttable == 1 || ttable == 2 || ttable == 4 || ttable == 6) {
//       root.subdivideWithRelativeError(maxDepth, 0.001);
//     }
//     else {
//       root.subdivideWithRelativeError(3, 0.1);
//     }
//
//     std::cout << "Octree Structure:\n";
//     roots.push_back(root);
//   }
//
// //     printOctreeStructure(&roots[0]);
//
//
//   table = 0;
//   Point3D searchPoint(0.11, 0.33, 0.55);
//
//   OctreeNode<Type>* result = roots[table].search(searchPoint);
//   // Display the result
//   cout << "table = " << table << endl;
  std::cout << "\nSearch Point: (" << searchPoint.x << ", " << searchPoint.y << ", " << searchPoint.z << ")\n";
  std::cout << "Smallest Sub-cube Bounds: ";
  std::cout << "(" << result->minBounds.x << ", " << result->minBounds.y << ", " << result->minBounds.z << ") to ";
  std::cout << "(" << result->maxBounds.x << ", " << result->maxBounds.y << ", " << result->maxBounds.z << ")\n";
  std::cout << "depth : = " << result->depth << " \n";
//         std::vector<std::vector<double>> interp_table;
//         for (size_t i = 0; i < result->corners.size(); ++i) {
//             interp_table.push_back(result->corners[i]);
//         }
//         std::cout << "Interp Table:\n";
//         for (const auto& corner : interp_table) {
//             std::cout << "(" << corner[0] << ", " << corner[1] << ", " << corner[2] << ", "<< corner[3] <<")\n";
//         }


  std::vector< double > interp_point_weights;
  std::vector<double>interp_point = {searchPoint.x, searchPoint.y, searchPoint.z};
  std::cout << "\n interp Point: (" << interp_point[0] << ", " << interp_point[1] << ", " << interp_point[2] << ")\n";

//         double interp_area = trilinier_interpolation(interp_table , interp_point);

  trilinier_interpolation_vector(result->corners, result->cornerWeights, interp_point, interp_point_weights);

  std::cout << "Interp weights:\n";
  for(size_t k = 0; k < interp_point_weights.size(); ++k) {
    const auto& entry = interp_point_weights[k];
    std::cout << " " << entry;
  }

  std::cout << "Interp weights error:\n";
  for(size_t k = 0; k < interp_point_weights.size(); ++k) {
    const auto& entry = (interp_point_weights[k] - weightCF[k]) / weightCF[k];
    std::cout << " " << entry;
  }

  Area0 = 0;
  Area = 0;
  Ix = 0;
  Iy = 0;
  Ix3 = 0;
  Ix2y = 0;
  Ixy2 = 0;
  Iy3 = 0;
  Ix2y2 = 0;

  for(unsigned ig = 0; ig < interp_point_weights.size(); ig++) {

    Area0 += gaussWeight[ig];
    Area += interp_point_weights[ig] * gaussWeight[ig];
    Ix += xg[ig] * interp_point_weights[ig] * gaussWeight[ig];
    Iy += yg[ig] * interp_point_weights[ig] * gaussWeight[ig];


    Ix3 += xg[ig] * xg[ig] * xg[ig] * interp_point_weights[ig] * gaussWeight[ig];
    Ix2y += xg[ig] * xg[ig] * yg[ig] * interp_point_weights[ig] * gaussWeight[ig];
    Ixy2 += xg[ig] * yg[ig] * yg[ig] * interp_point_weights[ig] * gaussWeight[ig];
    Iy3 += yg[ig] * yg[ig] * yg[ig] * interp_point_weights[ig] * gaussWeight[ig];

    Ix2y2 += xg[ig] * xg[ig] * yg[ig] * yg[ig] * interp_point_weights[ig] * gaussWeight[ig];
  }


  std::cout << "Area0 = " << Area0 << std::endl;
  std::cout << "Area = " << Area << std::endl;
  std::cout << "Ix = " << Ix << std::endl;
  std::cout << "Iy = " << Iy << std::endl;

  std::cout << "Ix3 = " << Ix3 << std::endl;
  std::cout << "Ix2y = " << Ix2y << std::endl;
  std::cout << "Ixy2 = " << Ixy2 << std::endl;
  std::cout << "Iy3 = " << Iy3 << std::endl;

  std::cout << "Ix2y2 = " << Ix2y2 << std::endl;

  return 0;


}
