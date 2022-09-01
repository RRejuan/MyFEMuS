
#include "MyEigenFunctions.hpp"
#include <numeric>
#include <iostream>

namespace femus {

  void FindBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, std::vector < double > &a, double &d) {

    const unsigned& dim = N.size();
    a.resize(dim);

    unsigned np = xp.size();
    Eigen::MatrixXd m(np, dim);

    std::vector < double > xg(dim, 0.);


    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }

      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          m(i, j) = sqrt((*w)[i]) * (xp[i][j] - xg[j]);
        }
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }
      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          m(i, j) = (xp[i][j] - xg[j]);
        }
      }
    }


    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();

    // use singular vector associated with min singular vector
    double aDotN = 0.;
    for(unsigned i = 0; i < dim; i++) {
      a[i] = v(i, dim - 1);
      aDotN += a[i] * N[i];
    }

    //Rotate normal by pi if Normal dot coefficents is less than zero
    if(aDotN < 0) {
      for(unsigned i = 0; i < dim; i++) {
        a[i] *= -1.;
      }
    }
  
    //Calculate constant d in ax+by+d=0 or ax+by+cz+d=0
    d = 0.;
    for(unsigned i = 0; i < dim; i++) {
      d -= a[i] * xg[i];
    }

// // use singular vector associated with min singular vector
//     for(unsigned i = 0; i < dim; i++) {
//       a[i] = v(i, dim - 1);
//     }
// 
// 
//     double aDotN = std::inner_product(a.begin(), a.end(), N.begin(), 0);
// 
//     //Rotate normal by pi if Normal dot coefficients is less than zero
//     if(aDotN < 0) {
//       for(unsigned i = 0; i < dim; i++) {
//         a[i] = -a[i];
//       }
//     }
// 
//     //Calculate constant d in ax+by+d=0 or ax+by+cz+d=0
//     d = -std::inner_product(a.begin(), a.end(), xg.begin(), 0);
  }
  
  void FindQuadraticBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, std::vector < double > &a) {
    const unsigned& dim = N.size();
    const unsigned nParam = 4 * dim - 2;
    a.resize(nParam);
    unsigned np = xp.size();
    Eigen::MatrixXd m(np, nParam);

    std::vector < double > xg(dim, 0.);
    
    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }
      
      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        unsigned cnt = 0;  
        for(int a = 2; a >= 0; a--){
          for(int b = a; b >= 0; b--){
            m(i, cnt) = sqrt((*w)[i]) * pow((xp[i][0] - xg[0]), b) * pow((xp[i][1] - xg[1]), a - b);
            cnt ++;
          }  
        }
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }
      //Fill matrix to be passed to JacobiSVD for Ax2 + Bxy + Cy2+ Dx + Ey + F = 0
      for(unsigned i = 0; i < np; i++) {
        unsigned cnt = 0;  
        for(unsigned a = 2; a <= 0; a--){
          for(unsigned b = a; b <= 0; b--){
            m(i, cnt) = pow((xp[i][0] - xg[0]), b) * pow((xp[i][1] - xg[1]), a - b);
            cnt ++;
          }
          if(cnt != nParam) {std::cerr<<"3D best fit not yet implemented!"; abort();}
        }  
      }
    }
    
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();
    
    // use singular vector associated with min singular vector
    double aDotN = 0.;
    for(unsigned i = 0; i < nParam; i++) {
      a[i] = v(i, nParam - 1);
//       aDotN += a[i] * N[i]; //TODO 
    }
    
//     //Rotate normal by pi if Normal dot coefficents is less than zero   TODO for quadric
//     if(aDotN < 0) {
//       for(unsigned i = 0; i < nParam; i++) {
//         a[i] *= -1.;
//       }
//     }
  }

  
}
