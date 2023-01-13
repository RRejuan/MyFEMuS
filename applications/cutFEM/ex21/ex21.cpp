#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <algorithm>    // std::sort
#include <ctime>
#include <cstdlib>
#include <climits>

#include <boost/math/special_functions/factorials.hpp>
//#include <boost/math/special_functions/pow.hpp>
using namespace std;
using boost::math::factorial;


#include <boost/multiprecision/cpp_bin_float.hpp>

namespace boost {
  namespace multiprecision {
    typedef number < backends::cpp_bin_float < 24, backends::digit_base_2, void, boost::int16_t, -126, 127 >, et_off >         cpp_bin_float_single;
    typedef number < backends::cpp_bin_float < 53, backends::digit_base_2, void, boost::int16_t, -1022, 1023 >, et_off >       cpp_bin_float_double;
    typedef number < backends::cpp_bin_float < 64, backends::digit_base_2, void, boost::int16_t, -16382, 16383 >, et_off >     cpp_bin_float_double_extended;
    typedef number < backends::cpp_bin_float < 113, backends::digit_base_2, void, boost::int16_t, -16382, 16383 >, et_off >    cpp_bin_float_quad;
    typedef number < backends::cpp_bin_float < 237, backends::digit_base_2, void, boost::int32_t, -262142, 262143 >, et_off >  cpp_bin_float_oct;
  }
} // namespaces

using boost::multiprecision::cpp_bin_float_oct;
using boost::multiprecision::cpp_bin_float_quad;



// double powr(const double &x, const int &y){
//   double x_p_y = 1.;
//   if (y > 0){
//     for(unsigned i=0; i < y; i++){
//       x_p_y *= x ;
//     }
//   }
//
//   else if(y<0) {
//     for(unsigned i=0; i < -y; i++){
//       x_p_y *= x ;
//     }
//     x_p_y = 1./x_p_y ;
//   }
//
//   else {
//   x_p_y = 1. ;
//   }
//   return x_p_y ;
// }


template <class Type>
void GetIntervalall(const std::vector <Type> &a1, const std::vector <Type> &a2, std::vector< std::pair<Type, Type> > &I1, std::vector< std::pair<Type, Type> > &I2, std::vector<std::pair<Type, Type>> &I3);


void random_polynomial(std::vector <double> &a1, std::vector <double> &a2) {
  a1.resize(3);
  a2.resize(3);
  a1[0] = ((double(std::rand()) / double(RAND_MAX)) * (4)) - 2;
  a1[1] = ((double(std::rand()) / double(RAND_MAX)) * (4)) - 2;
  a1[2] = ((double(std::rand()) / double(RAND_MAX)) * (4)) - 2;
  a2[0] = a1[0] ;
  a2[1] = ((double(std::rand()) / double(RAND_MAX)) * (4)) - 2;
  a2[2] = ((double(std::rand()) / double(RAND_MAX)) * (4)) - 2;
}

template <class Type>
Type integral_A2(const unsigned &m, const unsigned &n, const int &s, const Type &a, const Type &c, const std::vector <Type> &pol1, const std::vector< std::pair<Type, Type> > &I2) {
  std::vector <Type> k(3);
  k[0] = pol1[0] / (a * a);
  k[1] = pol1[1] / a;
  k[2] = k[0] * c * c - k[1] * c + pol1[2];
  k[1] -= 2 * c * k[0];

  std::vector <Type> A(s + n + 2, 0);
  std::vector <Type> B(s + n + 2, 0);


  Type kterms = (k[0] * k[2]) / (k[1] * k[1]);
  unsigned qMax = s + n + 1;

//   std::cout<< " ankor1 " << "k0 =" << k[0]<< "k1 =" << k[1]<< "k2 =" << k[2] << "kterms =" << kterms << "qMax =" << qMax << std::endl ;

  for(int q = 0; q <= qMax; q++) {
    Type term = 1;
    A[q] = term;
    unsigned q_p1_m2r = q + 1;
    unsigned qMax_mq_pr = qMax - q;
    for(int r = 1; r <= q / 2; r++) {
      q_p1_m2r -= 2;
      qMax_mq_pr += 1;
      //term *= k[0] * k[2] * (q - 2 * r + 1) * (q - 2 * r + 2) / (r * (s + n + 1 + r - q) * k[1] * k[1]);
      term *= kterms * q_p1_m2r * (q_p1_m2r + 1) / (r * qMax_mq_pr);
      A[q] += term ;
    }
    B[q] = A[q] * (pow(k[1], q) * pow(k[0], s + n + 1 - q)) / (factorial<Type>(q) * factorial<Type>(s + n + 1 - q));
    A[q] *= (pow(k[1], q) * pow(k[2], s + n + 1 - q)) / (factorial<Type>(q) * factorial<Type>(s + n + 1 - q));
//   std::cout<<"A["<<q<<"] = " << A[q] <<"  B[] ="<< B[q] << std::endl;
  }
  Type A2 = 0;
                                      //integration starts from here.....
  if(m >= qMax) {
    for(unsigned i = 0; i < I2.size(); i++)  {
      Type u1 = a * I2[i].first + c;
      Type u2 = a * I2[i].second + c;

//        std::cout<< " I2= "<< I2[i].first << ", "<< I2[i].second << std::endl;
//        std::cout<< " u1= "<< u1 << std::endl;
//        std::cout<< " u2= "<< u2 << std::endl;
// 1
      for(unsigned r = 0; r <= qMax; r++) {
//         std::cout<< " r= "<< r << std::endl;
        Type sum = 0;
        for(unsigned q = 0; q <= r; q++) {
//           std::cout<< " q= "<< q << std::endl;
//           std::cout<< " factorial<Type>(m - r + q)= "<< factorial<Type>(m - r + q) << std::endl;
//           std::cout<< " factorial<Type>(r - q)= "<< factorial<Type>(r - q) << std::endl;
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
//           std::cout<< " sum= "<< sum << std::endl;
        }
        int r_m_n = r - n;
//         std::cout<< " r_m_n = "<< r_m_n << std::endl;
        A2 += (r != n) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2 / u1));
//         std::cout<< " pow(u2, r - n)= "<< pow(u2, r_m_n) << std::endl;
//         std::cout<< " pow(u1, r - n)= "<< pow(u1, r_m_n) << std::endl;
//         std::cout<< " log(u2)= "<< log(u2) << std::endl;
//         std::cout<< " log(u1)= "<< log(u1) << std::endl;
//         std::cout<< " (log(u2/u1)= "<< (log(u2/u1)) << std::endl;
//
//         std::cout<< " r-n = "<< r-(n) << std::endl;
      }
     std::cout<< "1. A2= "<< A2 << std::endl;

// 2
      for(unsigned r = qMax + 1; r <= m; r++) {
        Type sum = 0;
        for(unsigned q = 0; q <= qMax; q++) {
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        int r_m_n = static_cast<int>(r) - static_cast<int>(n);
        A2 += sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
      }
         std::cout<< "2. A2= "<< A2 << std::endl;

// 3
      for(unsigned r = m + 1; r <= qMax + m; r++) {
        Type sum = 0;
        for(unsigned q = r - m; q <= qMax; q++) {
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        int r_m_n = r - n;
        A2 += sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
      }
         std::cout<< "3. A2= "<< A2 << std::endl;

// 4

      for(unsigned r = 0; r < qMax; r++) {
        Type sum = 0;
        for(unsigned q = 0; q <= r; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }
        std::cout<< "4. A2= "<< A2 << std::endl;

// 5
      for(unsigned r = qMax; r <= m; r++) {
        Type sum = 0;
        for(unsigned q = 0; q < qMax; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }
         std::cout<< "5. A2= "<< A2 << std::endl;

// 6
      for(unsigned r = m + 1; r < qMax + m; r++) {
        Type sum = 0;
        for(unsigned q = r - m; q < qMax; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }
         std::cout<< "6. A2= "<< A2 << std::endl;

//total
      A2 *= pow(-1, n) * factorial<Type>(n) * factorial<Type>(m) / pow(a, m);
//       std::cout<< "final. A2= "<< A2 << std::endl;

    }
    return A2;
  }



  else {

    for(unsigned i = 0; i < I2.size(); i++)  {
      Type u1 = a * I2[i].first + c;
      Type u2 = a * I2[i].second + c;
//       std::cout<< " u1= "<< u1 << std::endl;
//       std::cout<< " u2= "<< u2 << std::endl;
// 1
      for(unsigned r = 0; r <= m; r++) {
//         std::cout<< " r= "<< r << std::endl;
        Type sum = 0.;
        for(unsigned q = 0; q <= r; q++) {
//           std::cout<< " q= "<< q << std::endl;
//           std::cout<< " factorial<Type>(m - r + q)= "<< factorial<Type>(m - r + q) << std::endl;
//           std::cout<< " factorial<Type>(r - q)= "<< factorial<Type>(r - q) << std::endl;
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
//           std::cout<< " sum= "<< sum << std::endl;
        }
        int r_m_n = r - n;
//         std::cout<< " r_m_n = "<< r_m_n << std::endl;
        A2 += (r != n) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2 / u1));
//         std::cout<< " pow(u2, r - n)= "<< pow(u2, r_m_n) << std::endl;
//         std::cout<< " pow(u1, r - n)= "<< pow(u1, r_m_n) << std::endl;
//         std::cout<< " log(u2)= "<< log(u2) << std::endl;
//         std::cout<< " log(u1)= "<< log(u1) << std::endl;
//         std::cout<< " (log(u2/u1)= "<< (log(u2/u1)) << std::endl;
//
//         std::cout<< " r-n = "<< r-(n) << std::endl;
      }
      std::cout<< "1. A2= "<< A2 << std::endl;

// 2
      for(unsigned r = m + 1; r <= qMax; r++) {
        Type sum = 0.;
        for(unsigned q = r - m; q <= r; q++) {
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        int r_m_n = r - n;
        A2 += (r != n) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2 / u1));
      }
      std::cout<< "2. A2= "<< A2 << std::endl;

// 3
      for(unsigned r = qMax + 1; r <= qMax + m; r++) {
        Type sum = 0.;
        for(unsigned q = r - m; q <= qMax; q++) {
          sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        int r_m_n = r - n;
        A2 += sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
      }
      std::cout<< "3. A2= "<< A2 << std::endl;

// 41

      for(unsigned r = 0; r <= m; r++) {
        Type sum = 0.;
        for(unsigned q = 0; q <= r; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }
      std::cout<< "4. A2= "<< A2 << std::endl;

// 5
      for(unsigned r = m + 1; r < qMax; r++) {
        Type sum = 0.;
        for(unsigned q = r - m; q <= r; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }
      std::cout<< "5. A2= "<< A2 << std::endl;

// 6
      for(unsigned r = qMax; r < qMax + m; r++) {
        Type sum = 0.;
        for(unsigned q = r - m; q < qMax; q++) {
          sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
        }
        A2 += sum  * (pow(u2, qMax + s + m - r + 1) - pow(u1, qMax + s + m - r + 1)) / (qMax + s + m - r + 1);
      }      //           if (y>0 && y<1){
//             x[cnt] = y;
//             c++;
      std::cout<< "6. A2= "<< A2 << std::endl;

//total
      A2 *= pow(-1, n) * factorial<Type>(n) * factorial<Type>(m) / pow(a, m);
//       std::cout<< "final. A2= "<< A2 << std::endl;

    }
    return A2;;
  }

}

template <class Type>
Type integral_A3(const unsigned &m, const unsigned &n, const int &s, const Type &a, const Type &c, const std::vector <Type> &pol1, const std::vector< std::pair<Type, Type> > &I3) {
  Type A3 = 0.;
  std::vector <Type> k(3);

  k[0] = pol1[0] / (a * a);
  k[1] = pol1[1] / a;
  k[2] = k[0] * c * c - k[1] * c + pol1[2];
  k[1] -= 2 * c * k[0];


  for(int i=0; i <= s; i++){

    std::vector <Type> A(s -i + 1, 0);   // size of all this vector changes.
    std::vector <Type> B(s -i + 1, 0);


    Type kterms = (k[0] * k[2]) / (k[1] * k[1]);
    unsigned qMax = s - i;

//   std::cout<< " ankor1 " << "k0 =" << k[0]<< "k1 =" << k[1]<< "k2 =" << k[2] << "kterms =" << kterms << "qMax =" << qMax << std::endl ;

    for(int q = 0; q <= qMax; q++) {
      Type term = 1;
      A[q] = term;
      unsigned q_p1_m2r = q + 1;
      unsigned qMax_mq_pr = qMax - q;
      for(int r = 1; r <= q / 2; r++) {
        q_p1_m2r -= 2;
        qMax_mq_pr += 1;
        //term *= k[0] * k[2] * (q - 2 * r + 1) * (q - 2 * r + 2) / (r * (s + n + 1 + r - q) * k[1] * k[1]);
        term *= kterms * q_p1_m2r * (q_p1_m2r + 1) / (r * qMax_mq_pr);
        A[q] += term ;
      }
      B[q] = A[q] * (pow(k[1], q) * pow(k[0], qMax - q)) / (factorial<Type>(q) * factorial<Type>(qMax - q));
      A[q] *= (pow(k[1], q) * pow(k[2], qMax - q)) / (factorial<Type>(q) * factorial<Type>(qMax - q));

    }

    std::vector <Type> A3_part;
    A3_part.resize(i+1,0);

    if(m >= qMax) {
      for(unsigned w = 0; w < I3.size(); w++)  {
        Type u1 = a * I3[w].first + c;
        Type u2 = a * I3[w].second + c;
                               std::cout<< " u1= "<< u1 << std::endl;
                               std::cout<< " u2= "<< u2 << std::endl;
// 1
        for(unsigned r = 0; r <= qMax; r++) {
                                 std::cout<< " r= "<< r << std::endl;
          Type sum = 0.;
          for(unsigned q = 0; q <= r; q++) {
                                  std::cout<< " q= "<< q << std::endl;
                                  std::cout<< " factorial<Type>(m - r + q)= "<< factorial<Type>(m - r + q) << std::endl;
                                  std::cout<< " factorial<Type>(r - q)= "<< factorial<Type>(r - q) << std::endl;
              sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
                        //          std::cout<< " sum= "<< sum << std::endl;
          }
          int r_m_n = r+i+1;          //in A2 this was the power of u after integration. I am keeping the variable.
                        //         std::cout<< " r_m_n = "<< r_m_n << std::endl;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
                        //         std::cout<< " pow(u2, r - n)= "<< pow(u2, r_m_n) << std::endl;
                        //         std::cout<< " pow(u1, r - n)= "<< pow(u1, r_m_n) << std::endl;
                        //         std::cout<< " log(u2)= "<< log(u2) << std::endl;
                        //         std::cout<< " log(u1)= "<< log(u1) << std::endl;
                        //         std::cout<< " (log(u2/u1)= "<< (log(u2/u1)) << std::endl;
                        //
                        //         std::cout<< " r-n = "<< r-(n) << std::endl;
        }
          std::cout<< "1. A3_part= "<< A3_part[i]<< std::endl;

  // 2
        for(unsigned r = qMax + 1; r <= m; r++) {
          Type sum = 0.;
          for(unsigned q = 0; q <= qMax; q++) {
            sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = r+i+1;
          A3_part[i]+= sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
        }
          std::cout<< "2. A3_part= "<< A3_part[i]<< std::endl;

  // 3
        for(unsigned r = m + 1; r <= qMax + m; r++) {
          Type sum = 0.;
          for(unsigned q = r - m; q <= qMax; q++) {
            sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = r+i+1;
          A3_part[i]+= sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
        }
          std::cout<< "3. A3_part= "<< A3_part[i]<< std::endl;

  // 4

        for(unsigned r = 0; r < qMax; r++) {
          Type sum = 0.;
          for(unsigned q = 0; q <= r; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
          std::cout<< "4. A3_part= "<< A3_part[i]<< std::endl;

  // 5
        for(unsigned r = qMax; r <= m; r++) {
          Type sum = 0.;
          for(unsigned q = 0; q < qMax; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
          std::cout<< "5. A3_part= "<< A3_part[i]<< std::endl;

  // 6
        for(unsigned r = m + 1; r < qMax + m; r++) {
          Type sum = 0.;
          for(unsigned q = r - m; q < qMax; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
          std::cout<< "6. A3_part= "<< A3_part[i]<< std::endl;

      }

    }



    else {
      for(unsigned w = 0; w < I3.size(); w++)  {
        Type u1 = a * I3[w].first + c;
        Type u2 = a * I3[w].second + c;
                        //       std::cout<< " u1= "<< u1 << std::endl;
                        //       std::cout<< " u2= "<< u2 << std::endl;
// 1
        for(unsigned r = 0; r <= m; r++) {
                        //         std::cout<< " r= "<< r << std::endl;
          Type sum = 0.;
          for(unsigned q = 0; q <= r; q++) {
                        //           std::cout<< " q= "<< q << std::endl;
                        //           std::cout<< " factorial<Type>(m - r + q)= "<< factorial<Type>(m - r + q) << std::endl;
                        //           std::cout<< " factorial<Type>(r - q)= "<< factorial<Type>(r - q) << std::endl;
              sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
                        //           std::cout<< " sum= "<< sum << std::endl;
          }
          int r_m_n = r+i+1;          //in A2 this was the power of u after integration. I am keeping the variable.
                        //         std::cout<< " r_m_n = "<< r_m_n << std::endl;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
                        //         std::cout<< " pow(u2, r - n)= "<< pow(u2, r_m_n) << std::endl;
                        //         std::cout<< " pow(u1, r - n)= "<< pow(u1, r_m_n) << std::endl;
                        //         std::cout<< " log(u2)= "<< log(u2) << std::endl;
                        //         std::cout<< " log(u1)= "<< log(u1) << std::endl;
                        //         std::cout<< " (log(u2/u1)= "<< (log(u2/u1)) << std::endl;
                        //
                        //         std::cout<< " r-n = "<< r-(n) << std::endl;
        }
  //         std::cout<< "1. A3_part= "<< A3_part[i]<< std::endl;

  // 2
        for(unsigned r = m + 1; r <= qMax; r++) {
          Type sum = 0.;
          for(unsigned q = 0; q <= m; q++) {
            sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = r+i+1;
          A3_part[i]+= sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
        }
  //         std::cout<< "2. A3_part= "<< A3_part[i]<< std::endl;

  // 3
        for(unsigned r = qMax + 1; r <= qMax + m; r++) {
          Type sum = 0.;
          for(unsigned q = r - m; q <= qMax; q++) {
            sum += A[q] * pow(-c, m - r + q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = r+i+1;
          A3_part[i]+= sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n;
        }
  //         std::cout<< "3. A3_part= "<< A3_part[i]<< std::endl;

  // 4

        for(unsigned r = 0; r < m; r++) {
          Type sum = 0.;
          for(unsigned q = 0; q <= r; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
  //         std::cout<< "4. A3_part= "<< A3_part[i]<< std::endl;

  // 5
        for(unsigned r = m + 1; r < qMax; r++) {
          Type sum = 0.;
          for(unsigned q = r-m; q <= r; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
  //         std::cout<< "5. A3_part= "<< A3_part[i]<< std::endl;

  // 6
        for(unsigned r = qMax; r < qMax + m; r++) {
          Type sum = 0.;
          for(unsigned q = r - m; q < qMax; q++) {
            sum += B[q] * pow(-c, r - q) / (factorial<Type>(m - r + q) * factorial<Type>(r - q));
          }
          int r_m_n = qMax + s + m - r + 1;
          A3_part[i]+= (r_m_n != 0) ? sum  * (pow(u2, r_m_n) - pow(u1, r_m_n)) / r_m_n : sum  * (log(u2/u1));
        }
  //         std::cout<< "6. A3_part= "<< A3_part[i]<< std::endl;

      }

    }


  //total

      A3_part[i] = A3_part[i]/ ((n+i+1)*factorial<Type>(i));
  //       std::cout<< "final. A3= "<< A3 << std::endl;

      A3 += A3_part[i] ;

  }

  A3 *= factorial<Type>(m) / pow(a , m);
  return A3;

}




int main() {
  unsigned int m = 9;
  unsigned int n = 5;
  int s = 3;

  std::cout.precision(20);

  typedef double Type;

  std::vector <Type> pol1;
  std::vector <Type> pol2;

  std::srand(std::time(NULL));
  random_polynomial(pol1, pol2);

  Type a = pol2[1] - pol1[1];
  Type c = pol2[2] - pol1[2];


  pol1[0] = -1.53164;
  pol1[1] = -1.2347;
  pol1[2] = 1.0401;
  a = -0.527101;
  c = -2.00759;

  std::vector< std::pair <Type, Type> > I1;
  std::vector< std::pair <Type, Type> > I2;
  std::vector< std::pair <Type, Type> > I3;
  GetIntervalall(pol1, pol2, I1, I2, I3);
  
  I2.resize(1);
  I2[0].first = 0;
  I2[0].second = 0.514288;

  std::cout << pol1[0] << " " << pol1[1] << " " << pol1[2] << " " << a << " " << c << std::endl;
  for(unsigned i = 0; i < I2.size(); i++) {
    std::cout << I2[i].first << " " << I2[i].second << std::endl;
  }

  {
    clock_t t = clock();
    std::srand(std::time(NULL));
    for(unsigned i = 0; i < 1; i++) {
//      Type A2 = integral_A2(m, n, s, a, c, pol1, I2);
    }
    Type A2 = integral_A2(m, n, s, a, c, pol1, I2);

    std::cout << "double A2= " << A2 << std::endl;
    t = clock() - t;
    std::cout << "\nTime taken for predetermined cases: " << (Type)(t) / CLOCKS_PER_SEC << std::endl;
  }


  typedef cpp_bin_float_oct oct;
  
  //typedef cpp_bin_float_quad oct;

  oct ao = static_cast <oct>(a);
  oct co = static_cast <oct>(c);


  std::vector <oct> pol1o(3);
  pol1o[0] = static_cast <oct>(pol1[0]);
  pol1o[1] = static_cast <oct>(pol1[1]);
  pol1o[2] = static_cast <oct>(pol1[2]);

  std::vector <oct> pol2o(3);
  pol2o[0] = static_cast <oct>(pol2[0]);
  pol2o[1] = static_cast <oct>(pol2[1]);
  pol2o[2] = static_cast <oct>(pol2[2]);


  std::vector< std::pair<oct, oct> > I1o ;
  std::vector< std::pair<oct, oct> > I2o ;
  std::vector< std::pair<oct, oct> > I3o ;
  GetIntervalall(pol1o, pol2o, I1o, I2o, I3o);
  
  I2o.resize(1);
  I2o[0].first = 0;
  I2o[0].second = 0.514288;

  std::cout << "k = " << pol1o[0] << "; b = " << pol1o[1] << "; d = " << pol1o[2] << "; a = " << ao << "; c = " << co << ";" << std::endl;
  for(unsigned i = 0; i < I2o.size(); i++) {
    std::cout << "x1 = " << I2o[i].first << "; x2 = " << I2o[i].second << ";" << std::endl;
  }


  {
    clock_t t = clock();
    std::srand(std::time(NULL));
    for(unsigned i = 0; i < 1; i++) {
//      oct A2 = integral_A2(m, n, s, ao, co, pol1o, I2o);
    }
    oct A2 = integral_A2(m, n, s, ao, co, pol1o, I2o);

    std::cout << "oct A2= " << A2 << std::endl;
    t = clock() - t;
    std::cout << "\nTime taken for predetermined cases: " << (Type)(t) / CLOCKS_PER_SEC << std::endl;
  }



//   double eps = 2. / 3.e-14;
//   std::cout.precision(14);
//   std::cout << pow(eps, 10) << " " << pow(eps, 12) / pow(eps, 2) << " " <<  (1. / pow(eps, 2)) * pow(eps,12) << std::endl;

  return 1;
}


template <class Type>
int Sign(const Type &a) {
  return (a > 0) ? 1 : -1;
}

template <class Type>
void GetIntervalall(const std::vector <Type> &a1, const std::vector <Type> &a2, std::vector< std::pair<Type, Type> > &I1, std::vector< std::pair<Type, Type> > &I2, std::vector<std::pair<Type, Type>> &I3) {



  std::vector <Type> x(6);
  x[0] = 0 ;
  unsigned cnt = 1;
  Type delta;

  for(unsigned k = 0; k < 2; k++) {
    const std::vector<Type> &a = (k == 0) ? a1 : a2;
    delta = a[1] * a[1] - 4 * a[0] * a[2] ;
    if(delta >= 0) {
      Type sign = Sign(a[0]);
      for(unsigned i = 0; i < 2; i++) {
        Type y = (-a[1] - sign * sqrt(delta)) / (2 * a[0]) ;

        if(y >= 1) break;
        else if(y > 0) {
          x[cnt] = y ;
          cnt++;
        }
        sign *= -1;
      }

    }
  }
  x[cnt] = 1 ;
  cnt++;

  x.resize(cnt);
  std::sort(x.begin(), x.end());
  I1.resize(0);
  I2.resize(0);
  I3.resize(0);
  for(unsigned i = 0 ; i < cnt - 1 ; i++) {
    Type xm = (x[i] + x[i + 1]) / 2;
    Type f1 = a1[0] * xm * xm + a1[1] * xm + a1[2] ;
    Type f2 = a2[0] * xm * xm + a2[1] * xm + a2[2] ;
    if(f1 > 0) {
      if(f2 > 0) {
        I3.resize(I3.size() + 1, std::pair<Type, Type>(x[i], x[i + 1]));
      }
      else {
        I1.resize(I1.size() + 1, std::pair<Type, Type>(x[i], x[i + 1]));
      }
    }
    else if(f2 > 0) {
      I2.resize(I2.size() + 1, std::pair<Type, Type>(x[i], x[i + 1]));
    }
  }
}









// double b(const double &p) {
//   return std::max(std::min(p, 1.), 0.);
// }
//
// void GetIntervalall(const std::vector <double> &a1, const std::vector <double> &a2, std::vector< std::pair<double, double> > &I1, std::vector< std::pair<double, double> > &I2, std::vector<std::pair<double, double>> &I3) {
//
//   std::vector <double> x(6);
//   x[0] = 0 ;
//   unsigned cnt = 1;
//   for(unsigned k = 0; k < 2; k++) {
//     const std::vector<double> &a = (k == 0) ? a1 : a2;
//     double delta = a[1] * a[1] - 4 * a[0] * a[2] ;
//     if(delta > 0) {
//       for(unsigned i = 0; i < 2; i++) {
//         double y = (-a[1] - pow(-1, i) * sqrt(delta)) / (2 * a[0]) ;
//         if(y > 0 && y < 1) {
//           x[cnt] = y ;
//           cnt++ ;
//         }
//       }
//     }
//   }
//   x[cnt] = 1 ;
//   cnt++;
//
//   x.resize(cnt);
//   std::sort(x.begin(), x.end());
//   I1.resize(0);
//   I2.resize(0);
//   I3.resize(0);
//   for(unsigned i = 0 ; i < cnt - 1 ; i++) {
//     double xm = (x[i] + x[i + 1]) * 0.5 ;
//     double f1 = a1[0] * xm * xm + a1[1] * xm + a1[2] ;
//     double f2 = a2[0] * xm * xm + a2[1] * xm + a2[2] ;
//     if(f1 > 0) {
//       if(f2 > 0) {
//         I3.resize(I3.size() + 1, std::pair<double, double>(x[i], x[i + 1]));
//       }
//       else {
//         I1.resize(I1.size() + 1, std::pair<double, double>(x[i], x[i + 1]));
//       }
//     }
//     else if(f2 > 0) {
//       I2.resize(I2.size() + 1, std::pair<double, double>(x[i], x[i + 1]));
//     }
//   }
//
// //This part gives segmentation-fault-core-dumped if one of the region is empty. Solved : it had two problem
// // 1. every time it erases the size goes down but i increases.
// //2. for some reason in case of empty I1, it passes the first for loop. (i=0;i<-1;i++) . If we rewrite for loop as
// // for(unsigned i = 1; i < I1.size(); i++) {
// //     if(I1[i-1].second == I1[i].first) {
// //       I1[i-1].second = I1[i].second;
// //       I1.erase(I1.begin() + i );
// //       i--;
// //     }
// //   }
//
//   for(unsigned i = 1; i < I1.size(); i++) {
//     if(I1[i-1].second == I1[i].first) {
//       I1[i-1].second = I1[i].second;
//       I1.erase(I1.begin() + i );
//       i--;
//     }
//   }
//   for(unsigned i = 1; i < I2.size(); i++) {
//     if(I2[i-1].second == I2[i].first) {
//       I2[i-1].second = I2[i].second;
//       I2.erase(I2.begin() + i );
//       i--;
//     }
//   }
//   for(unsigned i = 1; i < I3.size(); i++) {
//     if(I3[i-1].second == I3[i].first) {
//       I3[i-1].second = I3[i].second;
//       I3.erase(I3.begin() + i );
//       i--;
//     }
//   }
// }
//
//
//
// void random_polynomial(std::vector <double> &a1, std::vector <double> &a2){
//     a1[0] = ((double(std::rand()) / double(RAND_MAX)) * (4)) -2;
//     a1[1] = ((double(std::rand()) / double(RAND_MAX)) * (4)) -2;
//     a1[2] = ((double(std::rand()) / double(RAND_MAX)) * (4)) -2;
//     a2[0] = a1[0] ;
//     a2[1] = ((double(std::rand()) / double(RAND_MAX)) * (4)) -2;
//     a2[2] = ((double(std::rand()) / double(RAND_MAX)) * (4)) -2;
// //         a1[0] = 1;
// //     a1[1] = -0.75;
// //     a1[2] = .125;
// //     a2[0] = a1[0] ;
// //     a2[1] = 0.75 ;
// //     a2[2] = -0.05;
//   }
//
// void get_roots(std::vector <double> a, double &delta, double &p, double &q ){
//     delta = a[1] * a[1] - 4 * a[0] * a[2] ;
// //   std::cout <<"delta= "<< delta << std::endl;
//     if(delta > 0) {
//         p = (-a[1] + sqrt(delta)) / (2 * a[0]) ;
//         q = (-a[1] - sqrt(delta)) / (2 * a[0]) ;
//         if(p > q) {
//           std::swap(p,q);
//         }
// //         std::cout <<"roots are  "<< p << " and "<< q << std::endl;
//     }
// }





/*
int main() {
      double p1=0,p2=0,q1=0,q2=0;
      std::vector <double> a2(3), a1(3);
      std::vector< std::pair<double, double> > I1, I2, I3 ;

      clock_t t = clock();
      std::srand(std::time(NULL));
  for(unsigned i=0;i<10;i++){
      random_polynomial(a1,a2);
        std::cout <<"polynomial p(x) = "<< a1[0] << "x^2 + (" << a1[1] << "x) + (" << a1[2] << ") " <<std::endl;
        std::cout <<"polynomial q(x) = "<< a2[0] << "x^2 + (" << a2[1] << "x) + (" << a2[2] << ") " <<std::endl;

      double delta1,delta2;
      get_roots(a1,delta1,p1,p2);
      get_roots(a2,delta2,q1,q2);

        std::cout <<"roots p1 = " << p1 << " & p2 = " << p2 << "d="<<delta1 <<std::endl;
        std::cout <<"roots q1 = " << q1 << " & q2 = " << q2 << "d="<<delta2 <<std::endl;

      GetIntervalall(a1,a2,I1,I2,I3);

      std::cout << "I1= " ;
      for (unsigned i=0 ; i<I1.size(); i++){
         std::cout << "(" << I1[i].first << "," << I1[i].second <<") U " ;
      }
      std::cout << "\nI2= " ;
      for (unsigned i=0 ; i<I2.size(); i++){
         std::cout << "(" << I2[i].first << "," << I2[i].second <<") U " ;
      }

      std::cout << "\nI3= " ;
      for (unsigned i=0 ; i<I3.size(); i++){
         std::cout << "(" << I3[i].first << "," << I3[i].second <<") U " <<std::endl;
      }
      t = clock() - t;
      std::cout << "\nTime taken for generalized algorithm : " << (double)(t)/ CLOCKS_PER_SEC << std::endl;

      if (delta1 > 0){
        if(delta2 > 0){
          GetInterval4Old( p1, p2, q1, q2, a1[0], I1, I2, I3);
        }
        else {
          bool pIsComplex=0;
          GetInterval2(p1, p2, pIsComplex, a1[0], I1, I2, I3);
        }
      }
      else{
        if (delta2 > 0){
          bool pIsComplex=1;
          GetInterval2(q1, q2, pIsComplex, a1[0], I1, I2, I3);
        }
        else {
           GetInterval0(a1[0], I1, I2, I3);
        }
      }
      std::cout << "I1= " ;
      for (unsigned i=0 ; i<I1.size(); i++){
        std::cout << "(" << I1[i].first << "," << I1[i].second <<") U " ;
      }
      std::cout << "\nI2= " ;
      for (unsigned i=0 ; i<I2.size(); i++){
         std::cout << "(" << I2[i].first << "," << I2[i].second <<") U " ;
      }
      std::cout << "\nI3= " ;
      for (unsigned i=0 ; i<I3.size(); i++){
         std::cout << "(" << I3[i].first << "," << I3[i].second <<") U " ;
      }
   t = clock() - t;
   std::cout << "\nTime taken for predetermined cases: " <<(double)(t)/ CLOCKS_PER_SEC << std::endl;
  }
  return 1;
}*/
