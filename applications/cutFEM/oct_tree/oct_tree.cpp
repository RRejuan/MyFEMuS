
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

template <class Type>
struct PointT {
    Type x;
    Type y;
};

template <class Type>
struct Parabola {
    Type k;
    Type b;
    Type d;
};

template <class Type>
void swap(Type& x, Type& y) {
  Type temp = x;
  x = y;
  y = temp;
}

template <class Type>
struct Point {
    Type x;
    Type y;
};

template <class Type>
struct Circle {
    Point<Type> center;
    Type radius;
};

template <class TypeIO, class TypeA>
void GetIntervalall(const std::vector <TypeIO> &a1, const std::vector <TypeIO> &a2, std::vector< std::pair<TypeIO, TypeIO> > &I1, std::vector< std::pair<TypeIO, TypeIO> > &I2, std::vector<std::pair<TypeIO, TypeIO>> &I3) {
  I1.resize(0);
  I2.resize(0);
  I3.resize(0);
  std::vector <TypeA> x(6);
  x[0] = 0;

  unsigned cnt = 1;
  TypeA delta;
  std::vector<TypeA> a = {static_cast<TypeA>(a1[0]), static_cast<TypeA>(a1[1]), static_cast<TypeA>(a1[2])};
  for(unsigned k = 0; k < 2; k++) {
    if( k == 1){
      a = {static_cast<TypeA>(a2[0]), static_cast<TypeA>(a2[1]), static_cast<TypeA>(a2[2])};
    }

    if (a[0] == 0){
      TypeA y = -a[2] / a[1];
      if(y < 1 && y > 0) {
        x[cnt] = y;
        cnt++;
      }
    }
    else {
      delta = static_cast<TypeA>(a[1] * a[1] - 4 * a[0] * a[2]);
      if(delta >0) {
        TypeA sqrtdelta = sqrt(delta);
        int sign = (a[0] > 0) ? 1 : -1;

        for(unsigned i = 0; i < 2; i++) {  //TODO
            TypeA y = (-a[1] - sign * sqrtdelta) / (2 * a[0]);
            if(y >= 1) break;
            else if(y > 0) {
              x[cnt] = y;
              cnt++;
            }
          sign *= -1;
        }
      }
    }
  }
  x[cnt] = 1 ;
  cnt++;

  x.resize(cnt);
  std::sort(x.begin() + 1, x.end() - 1); //TODO
//   for(unsigned i = 0; i < cnt; i++) {
// //     std::cout << "x = " << x[i] << std::endl;
//   }
  for(unsigned i = 0 ; i < cnt - 1 ; i++) {
    TypeA xm = (x[i] + x[i + 1]) / 2;
    TypeA f1 = a1[0] * xm * xm + a1[1] * xm + a1[2] ;
    TypeA f2 = a2[0] * xm * xm + a2[1] * xm + a2[2] ;
    if(f1 > 0) {
      if(f2 > 0) {
        I3.resize(I3.size() + 1, std::pair<TypeIO, TypeIO>(static_cast<TypeIO>(x[i]), static_cast<TypeIO>(x[i + 1])));
      }
      else {
        I1.resize(I1.size() + 1, std::pair<TypeIO, TypeIO>(static_cast<TypeIO>(x[i]), static_cast<TypeIO>(x[i + 1])));
      }
    }
    else if(f2 > 0) {
      I2.resize(I2.size() + 1, std::pair<TypeIO, TypeIO>(static_cast<TypeIO>(x[i]), static_cast<TypeIO>(x[i + 1])));
    }
  }

// Reduce size for adjacent intervals
  for(unsigned i = 1; i < I1.size(); i++) {
    if(I1[i - 1].second == I1[i].first) {
      I1[i - 1].second = I1[i].second;
      I1.erase(I1.begin() + i);
      i--;
    }
  }
  for(unsigned i = 1; i < I2.size(); i++) {
    if(I2[i - 1].second == I2[i].first) {
      I2[i - 1].second = I2[i].second;
      I2.erase(I2.begin() + i);
      i--;
    }
  }
  for(unsigned i = 1; i < I3.size(); i++) {
    if(I3[i - 1].second == I3[i].first) {
      I3[i - 1].second = I3[i].second;
      I3.erase(I3.begin() + i);
      i--;
    }
  }

}

template <class Type>
Parabola <Type>  get_parabola_equation( const PointT <Type> p1,const PointT <Type> p2,const PointT <Type> p3) {
    Type x1 = p1.x, x2 = p2.x, x3 = p3.x;
    Type y1 = p1.y, y2 = p2.y, y3 = p3.y;
    Type k,b,d;
    Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x);

//     Type det = x1 * x1 * (x2 - x3) -x1* (x2*x2 - x3*x3)+ x2*x3*(x2 - x3) ;
//     Type denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
    if(fabs(det) >0.0000000000000001){
       k = -(y1 * (x2 - x3) + y2 * (x3 - x1) + y3 * (x1 - x2)) / det;
       b = -(y1 * (x3*x3 - x2*x2) + y2 * (x1*x1 - x3*x3)+ y3 * ((x2*x2 - x1*x1))) / det;
       d = -(y1 * x2 * x3 * (x2 - x3) + y2 * x3 * x1 * (x3 - x1) + y3 * x1 * x2 * (x1 - x2)) / det;
    }
    else{
      Type slope = (y1-y2)/(x1-x2) ;
      k=0;
      b= -slope;
      d= x2*slope - y2 ;
    }

//     else {This will give us a straight line paralal to y axix.
//        k = -0.;
//        b = -1.;
//        d = p1.x ;
//     }

    if (fabs(k) < 1.e-16) k = 0 ;
    if (fabs(b) < 1.e-16) b = 0 ;
    if (fabs(d) < 1.e-16) d = 0 ;

    return {k, b, d};
}

template <class Type>
void CheckIntersection(int &intersect_number, unsigned int &table_number , std::vector <Type> &intersection, std::vector <Type> &interp_point, const Parabola <Type> &parabola){

  table_number = -1;
  intersect_number=0;
  intersection.resize(0);
  interp_point.resize(0);
  Type k = parabola.k;
  Type b = parabola.b;
  Type d = parabola.d;
  Type c = 1;
  int left =0 , top = 0, right = 0, bottom = 0 ;
//   cout<< " parabola I get from solving system of linear equation :  " << parabola.k <<"x^2 + "<< parabola.b <<"x + "<< parabola.d << "+" <<  c << " y=0"  <<endl;

      if (-d>=0 && -d<=1){ //LEFT
        intersection.resize(intersection.size()+2);
        intersection[intersection.size()-2] = 0;
        intersection[intersection.size()-1] = -d;
        left = 1 ;
        intersect_number += 1;
        interp_point.resize(interp_point.size()+1);
        interp_point[interp_point.size()-1] = -d;
//         cout << " left = " << left ;
      }
      // LEFT-TOP solve kx^2+bx+d-1 =0  ; Table 0
      if (k == 0){
      Type x =  (-1-d)/b ;
        if(x <= 1 && x>= 0) {
          intersection.resize(intersection.size()+2);
          intersection[intersection.size()-2] = x;
          intersection[intersection.size()-1] = 1;
          interp_point.resize(interp_point.size()+1);
          interp_point[interp_point.size()-1] = x;
          top =1;
          intersect_number += 1;
          if (left ==1) table_number = 0 ;
//           cout << " top = " << top ;
        }
      }
      else {
        Type delta = b*b - 4*k*(d+1);
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;

              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                if(x > 1) break;
                else if(x >= 0) {
                  intersection.resize(intersection.size()+2);
                  intersection[intersection.size()-2] = x;
                  intersection[intersection.size()-1] = 1;
                  interp_point.resize(interp_point.size()+1);
                  interp_point[interp_point.size()-1] = x;
                  intersect_number += 1;
                  if (top ==1){table_number = 3 ;}
                  top += 1;
                  if (left ==1){table_number = 0 ;}
    //               cout << " top = " << top ;
                }
                sign *= -1;
              }
            }
      }
      Type y_1=-(k+b+d); //LEFT-RIGHT x=1 ; Table 1
      if (y_1 >= 0 && y_1 <= 1){ //TODO check sign when normal change
          intersection.resize(intersection.size()+2);
          intersection[intersection.size()-2] = 1;
          intersection[intersection.size()-1] = y_1;
          interp_point.resize(interp_point.size()+1);
          interp_point[interp_point.size()-1] = y_1;
          intersect_number += 1;
          if (left ==1){table_number = 1 ;}
          if  (top >=1){table_number = 4 ;}
          right = 1 ;
//           cout << " right = " << right ;
      }

        //LEFT-BOTTOM  solve kx^2+bx+d =0 ; Table 2
      if (k == 0){
          Type x =  -d/b ;
          if(x <= 1 && x>= 0) {
            intersection.resize(intersection.size()+2);
            intersection[intersection.size()-2] = x;
            intersection[intersection.size()-1] = 0;
            interp_point.resize(interp_point.size()+1);
            interp_point[interp_point.size()-1] = x;
            intersect_number += 1;
            if (left == 1){table_number = 2 ;}
            if (right == 1){table_number = 6 ;}
            if (top >= 1){table_number = 5 ;}
            bottom = 1;
//             cout << " bottom = " << bottom ;
          }
      }

      else {
          Type delta = b*b - 4*k*d;
          if(delta >=0) {
            Type sqrtdelta = sqrt(delta);
            int sign = (k > 0) ? 1 : -1;

            for(unsigned i = 0; i < 2; i++) {
              Type x = (- b - sign * sqrtdelta) / (2 * k);
//               cout << " bottom root = " << x ;
              if(x > 1) break;
              else if(x >= 0) {
                intersection.resize(intersection.size()+2);
                intersection[intersection.size()-2] = x;
                intersection[intersection.size()-1] = 0;
                interp_point.resize(interp_point.size()+1);
                interp_point[interp_point.size()-1] = x;
                if (bottom >=1){table_number = 7 ;}
                if (left ==1){table_number = 2 ;}
                if (right ==1){table_number = 6 ;}
                if (top ==1){table_number = 5 ;}    // TODO check the table
                bottom += 1;
                intersect_number += 1;
//                 cout << " bottom = " << bottom ;
              }
              sign *= -1;
            }
          }
      }

      if (intersect_number == 4){
        if(left == 1 && top == 2 && right == 1){
          table_number = 0;
          Type swap;
          swap = interp_point[2];
          interp_point[2] = interp_point[3];
          interp_point[3] = swap;
        }
        else if (left == 1 && top == 2 && bottom == 1){
          table_number = 1;
          Type swap;
          swap = interp_point[1];
          interp_point[1] = interp_point[2];
          interp_point[2] = interp_point[3];
          interp_point[3] = swap;
        }

        else if(left == 1 && bottom == 2 && right ==1){
          table_number = 2;
          Type swap;
          swap = interp_point[2];
          interp_point[2] = interp_point[1];
          interp_point[1] = swap;
        }

        else if (left == 1 && bottom == 2 && top == 1){
          table_number = 3;
          Type swap;
          swap = interp_point[1];
          interp_point[1] = interp_point[2];
          interp_point[2] = interp_point[3];
          interp_point[3] = swap;
        }

        else if (right == 1 && top == 2 && bottom == 1){
          table_number = 4;
          Type swap;
          swap = interp_point[1];
          interp_point[1] = interp_point[0];
          interp_point[0] = interp_point[2];
          interp_point[2] = interp_point[3];
          interp_point[3] = swap;
        }

        else if (right == 1 &&  bottom == 2 && top == 1){
          table_number = 5;
          Type swap;
          swap = interp_point[2];
          interp_point[2] = interp_point[0];
          interp_point[0] = interp_point[1];
          interp_point[1] = interp_point[3];
          interp_point[3] = swap;
        }

        else if( top == 2 && bottom == 2){
//           if(bottom == 1) table_number = 2;
//           else if(top == 1 ) table_number =3;
          if (interp_point[0] > interp_point[3]){
            table_number = 6 ;
            Type swap;
            swap = interp_point[0];
            interp_point[0] = interp_point [3];
            interp_point[3] = swap;
          }
          else{
            table_number = 7;
            Type swap;
            swap = interp_point[1];
            interp_point[1] = interp_point [2];
            interp_point[2] = swap;

          }
        }

      }
/*
cout<< " " << " left " << left << " top "<< top << " right "<< right << " bottom " << bottom  << " table number :"<< table_number << " number of intersection " << intersect_number <<endl;*/

}


template <class Type>
Type easy_integral_A2(const unsigned &m, const unsigned &n, const int &s, const Type &a, const Type &c, const std::vector <Type> &pol1, const std::vector< std::pair<Type, Type> > &I2) {

  Type A2(0);
  if(a == 0) {
    int rMax = s + n + 1;
    for(unsigned i = 0; i < I2.size(); i++) {
      unsigned r_pm_p1 = m + 1;
      unsigned rMax_mr_pm_p1 = 2 * rMax + m + 1;

//       BEGIN preevaluating
        std::vector <Type> diff_x_pow(2 * rMax + m + 2, 0) ;
//         Type x1pi = I2[i].first;
//         Type x2pi = I2[i].second;
        Type x1pi = pow(I2[i].first,m+1);
        Type x2pi = pow(I2[i].second,m+1);
        for(unsigned pwr = m+1; pwr <= 2 * rMax + m + 1 ; pwr++, x1pi *= I2[i].first, x2pi *= I2[i].second) {
            diff_x_pow[pwr] = (x2pi - x1pi) / (pwr) ;
        }
//         END

    Type pterm = pol1[0] * pol1[2];

      for(int r = 0; r <= rMax; r++) {
        Type term(1);
        Type sum1 = pow(pol1[1], r);
        Type sum2(0);
        unsigned r_p1_m2p =  r + 1;
        unsigned rMax_mr_pp = rMax - r;
        for(int p = 1; p <= r / 2; p++) {
          r_p1_m2p -= 2;
          rMax_mr_pp += 1;
  //           sum += (pow(pol1[0], p) * pow(pol1[1], r - 2 * p) * pow(pol1[2], rMax + p - r)) / (factorial<Type>(p) * factorial<Type>(r - 2 * p) * factorial<Type>(rMax + p - r));
  //           term *= pol1[0] * pol1[2] *(r - 2 * p + 1)*(r - 2 * p + 2) / ( p * (rMax + p - r));
          term *= pterm * r_p1_m2p * (r_p1_m2p + 1) / (p * rMax_mr_pp);
          sum1 += term * pow(pol1[1], r - 2 * p);
        }
        sum1 = sum1 / (factorial<Type>(r) * factorial<Type>(rMax - r));
        sum2 = (r == rMax) ? 0 : sum1 * pow(pol1[0], rMax - r);
        sum1 *= pow(pol1[2], rMax - r);
//         for(unsigned i = 0; i < I2.size(); i++) {

        A2 += sum1 * diff_x_pow[r_pm_p1] +  sum2 * diff_x_pow[rMax_mr_pm_p1] ;
//           A2 += sum1 * (pow(I2[i].second, r_pm_p1) - pow(I2[i].first, r_pm_p1)) / r_pm_p1 +  sum2 * (pow(I2[i].second, rMax_mr_pm_p1) - pow(I2[i].first, rMax_mr_pm_p1)) / rMax_mr_pm_p1 ;
//         }
        r_pm_p1 ++;
        rMax_mr_pm_p1 --;
      }
    }

    A2 *= ((n % 2 == 0) ? -1 : 1) * factorial<Type>(n) / pow(c, n + 1);

    return A2;
  }
  else {
    std::vector <Type> k(3);
    std::cout.precision(20);

    k[0] = pol1[0] / (a * a);
    k[1] = pol1[1] / a;
    k[2] = k[0] * c * c - k[1] * c + pol1[2];
    k[1] -= 2 * c * k[0];

    std::vector <Type> A(s + n + 2, 0);
    std::vector <Type> B(s + n + 2, 0);

    unsigned qMax = s + n + 1;

    //BEGIN pre-evalate A[q] and B[q].
    if(k[1] != 0) {  //regular
      Type kterms = (k[0] * k[2]) / (k[1] * k[1]);
      for(int q = 0; q <= qMax; q++) {
        Type term(1);
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
        A[q] *= pow(k[1], q) / (factorial<Type>(q) * factorial<Type>(qMax - q));
        B[q] = A[q] * (pow(k[0], qMax - q));
        A[q] *= pow(k[2], qMax - q) ;
      }
    }

      //END pre-evalate A[q] and B[q].

    else { // (special case if k[1] =  small )
      for(unsigned w = 0; w < I2.size(); w++)  {
        Type u1 = a * I2[w].first + c;
        Type u2 = a * I2[w].second + c;
//         k[2] = pol1[2] - pol1[1]*c /(2*a);
        for(int p = 0; p <= m; p++) {
          Type sum(0);
          for(int q = 0; q <= qMax; q++) {
            int pwr = 2 * q - n + p ;
            sum += pow(k[2], qMax - q) * pow(k[0], q) / (factorial<Type>(q) * factorial<Type>(qMax - q)) * ((pwr == 0) ? log(u2 / u1) : ((pow(u2, pwr) - pow(u1, pwr)) / (pwr))) ;
          }
          A2 += sum * pow(-c, m - p) / (factorial<Type>(p) * factorial<Type>(m - p)) ;
        }
      }
      A2 *= pow(-1, n + 1) * factorial<Type>(n) * factorial<Type>(m) / pow(a, m + 1) ;

      return A2;
    }


    //integration starts from here.....
    for(unsigned w = 0; w < I2.size(); w++)  {
      Type u1 = a * I2[w].first + c;
      Type u2 = a * I2[w].second + c;

      if(u1 == 0 || u2 == 0) {   // TODO need to fix this. if we have double parts of any region. This is wrong .

        Type A2i(0);
        Type c_0 = (a * pol1[1] - pol1[0] * c) / (a * a);
        int pMax = s + n + 1 ;
        Type p_term(1);
        Type p_sum(1);
        for(int p = 1; p <= pMax; p++) {
          Type q_term(1);
          Type q_sum = q_term;

          for(int q = 1; q <= s; q++) {
            Type r_pm_p1 = p + q + 1;
            q_term *= a * (s - q + 1) / (c * q);
            q_sum += q_term * (pow(I2[w].second, r_pm_p1) - pow(I2[w].first, r_pm_p1)) / r_pm_p1;
          }
          q_sum *= pow(c, s) / factorial<Type>(s) ;
          p_term *= pol1[0] * (pMax - p + 1) / (a * p * c_0);
          p_sum += p_term * q_sum ;
        }

        A2 += p_sum * ((n % 2 == 0) ? -1 : 1) * factorial<Type>(n) * factorial<Type>(s) * pow(c_0, pMax) / factorial<Type>(pMax) ;
      }
      else {

//        {
// //         Type A2i(0);
// //         for(unsigned p = 0; p <= m; p++) {
// //           Type sum1(0);
// //           for(unsigned q = 0; q <= qMax; q++) {
// //             int pwr = p + q - n;
// //             sum1 += A[q] * ((pwr== 0) ? log(u2 / u1) : (pow(u2, pwr) - pow(u1, pwr)) / (pwr));
// //           }
// //           Type sum2(0);
// //           for(unsigned q = 0; q < qMax; q++) {
// //             int pwr= 2 * s + n + 2 + p - q;
// //             sum2 += B[q] * (pow(u2,pwr) - pow(u1,pwr)) / (pwr);
// //           }
// //           A2i += (sum1 + sum2) * pow(-c, m - p) / (factorial<Type>(p) * factorial<Type>(m - p));
// //         }
// //         A2 += A2i * pow(-1, n + 1) * factorial<Type>(n) * factorial<Type>(m) / pow(a, m + 1) ;
//        }

        // BEGIN pre evalution of power of U
        std::vector <Type> diff_u_pow(2 * s + 2 * n + m + 3, 0) ;
        Type u1pi = 1. / pow(u1, n);
        Type u2pi = 1. / pow(u2, n);
        for(int pwr = 0; pwr <= n - 1 ; pwr++, u1pi *= u1, u2pi *= u2) {
          int actual_pwr = pwr - n;
          diff_u_pow[pwr] = (u2pi - u1pi) / actual_pwr ;
        }
//             Type u1pi = 1./u1;
//             Type u2pi = 1./u2;
//             for(int pwr = n-1; pwr >= 0 ; pwr--, u1pi /= u1, u2pi /= u2) {
//               int actual_pwr = pwr - n;
//               diff_u_pow[pwr] = (u2pi - u1pi) / actual_pwr ;
//             }

        diff_u_pow[n] = log(u2 / u1) ;
        u1pi = u1;
        u2pi = u2;
        for(int pwr = n + 1; pwr <= 2 * qMax + m ; pwr++, u1pi *= u1, u2pi *= u2) {
          int actual_pwr = pwr - n;
          diff_u_pow[pwr] = (u2pi - u1pi) / actual_pwr ;
        }
        // END pre evaluation of power


        Type A2i(0);
        for(int p = 0; p <= m; p++) {
          Type sum1(0);
          for(int q = 0; q <= qMax; q++) {
//             int pwr = p + q;                       // added n with original power
//             sum1 += A[q] * diff_u_pow[pwr] ;
            sum1 += A[q] * diff_u_pow[p + q] ;

          }
          Type sum2(0);
          for(int q = 0; q < qMax; q++) {
//             int pwr = 2 * qMax + p - q;            // added n with original power
//             sum2 += B[q] * diff_u_pow[pwr];
            sum2 += B[q] * diff_u_pow[2 * qMax + p - q];
          }
          A2i += (sum1 + sum2) * pow(-c, m - p) / (factorial<Type>(p) * factorial<Type>(m - p));
        }
        A2 += A2i * ((n % 2 == 0) ? -1 : 1) */* pow(-1, n + 1) **/ factorial<Type>(n) * factorial<Type>(m) / pow(a, m + 1) ;
      }

    }
    //total
    //  A2 *= pow(-1, n + 1) * factorial<Type>(n) * factorial<Type>(m) / pow(a, m + 1);
// //           std::cout << "final. A2= " << A2 << std::endl;
    return A2;
  }
}


template <class Type>
Type easy_integral_A3(const unsigned &m, const unsigned &n, const int &s, const Type &a, const Type &c, const std::vector <Type> &pol1, const std::vector< std::pair<Type, Type> > &I3) {
  Type A3(0);
  if(a == 0) {
    for(int i = 0; i <= s; i++) {
      for(unsigned w = 0; w < I3.size(); w++) {
        int pMax = s - i;
        for(int r = 0; r <= pMax; r++) {
          Type sum1 = pow(pol1[2], pMax - r)/factorial<Type>(pMax - r) ;
          Type sum = sum1 * (pow(I3[w].second,2 * r + m + 1) - pow(I3[w].first, 2 * r + m + 1))/ (2 * r + m + 1);
          for(int p = 1; p <= pMax - r; p++) {
            sum1 *= pol1[1] * (pMax - r - p + 1) / (pol1[2] * p)   ;
            sum += sum1 * (pow(I3[w].second,2 * r + m+p + 1) - pow(I3[w].first, 2 * r +p+ m + 1))/ (2 * r + m+p + 1) ;
          }
          A3 += sum * pow(pol1[0], r) / (factorial<Type>(r) * (n + i + 1) * factorial<Type>(i)) ;
        }
      }
    }
  }

//     if(a == 0) { //preevaluate this but it is expensive!!!!
//     for(int i = 0; i <= s; i++) {
//       for(unsigned w = 0; w < I3.size(); w++) {
//         //BEGIN preevaluating
// //         std::vector <Type> diff_x_pow(2 * s + 2, 0) ;
// //         Type x1pi = pow(I3[w].first,m+1);
// //         Type x2pi = pow(I3[w].second,m+1);
// //         for(unsigned pwr = m+1; pwr <= 2 * s + m + 1 ; pwr++, x1pi *= I3[w].first, x2pi *= I3[w].second) {
// //             //               diff_u_pow[pwr] = (pow(u2, pwr) - pow(u1, pwr)) / (pwr) ;
// //             diff_x_pow[pwr-m] = (x2pi - x1pi) / (pwr) ;
// //         }
//         //END
//         int pMax = s - i;
//         // #1
//         for(int r = 0; r <= pMax; r++) {
//
//           Type sum1 = pow(pol1[2], pMax - r)/factorial<Type>(pMax - r) ;
// //           Type sum = sum1 * diff_x_pow[2 * r + 1];
// //           int r_pm_p1 = 2 * r + m + 1;
// //           Type sum = sum1 * (pow(I3[w].second,r_pm_p1) - pow(I3[w].first, r_pm_p1))/ (r_pm_p1);
//           Type sum = sum1 * (pow(I3[w].second,2 * r + m + 1) - pow(I3[w].first, 2 * r + m + 1))/ (2 * r + m + 1);
//
//           for(int p = 1; p <= pMax - r; p++) {
// //             r_pm_p1++;
//             sum1 *= pol1[1] * (pMax - r - p + 1) / (pol1[2] * p)   ;
// //             sum += sum1 * diff_x_pow[2 * r + p + 1] ;
//             sum += sum1 * (pow(I3[w].second,2 * r + m+p + 1) - pow(I3[w].first, 2 * r +p+ m + 1))/ (2 * r + m+p + 1) ;
// //          sum += sum1 * (pow(I3[w].second,r_pm_p1) - pow(I3[w].first, r_pm_p1))/(r_pm_p1) ;
// //             sum += pow(pol1[1], p) * pow(pol1[2], pMax - r - p) * diff_x_pow[r_pm_p1] / (factorial<Type>(p) * factorial<Type>(pMax - r - p) ) ;
//           }
//           A3 += sum * pow(pol1[0], r) / (factorial<Type>(r) * (n + i + 1) * factorial<Type>(i)) ;
//         }
//       }
//     }
//   }
//     if(a == 0) { // TODO optimize
//     for(int i = 0; i <= s; i++) {
//       for(unsigned w = 0; w < I3.size(); w++) {
//         int pMax = s - i;
//         // #1
//         for(int r = 0; r <= pMax; r++) {
//           Type sum = 0;
//
//           for(int p = 0; p <= pMax - r; p++) {
//             Type r_pm_p1 = 2 * r + p + m + 1;
//             sum += pow(pol1[1], p) * pow(pol1[2], s - i - r - p) * (pow(I3[w].second, r_pm_p1) - pow(I3[w].first, r_pm_p1)) / (factorial<Type>(p) * factorial<Type>(s - i - r - p) * r_pm_p1) ;
//           }
//           A3 += sum * pow(pol1[0], r) / (factorial<Type>(r) * (n + i + 1) * factorial<Type>(i)) ;
//         }
//       }
//     }
//   }

  else {
    std::vector <Type> k(3);
    k[0] = pol1[0] / (a * a);
    k[1] = pol1[1] / a;
    k[2] = k[0] * c * c - k[1] * c + pol1[2];
    k[1] -= 2 * c * k[0];

    for(int i = 0; i <= s; i++) {
      unsigned qMax = s - i;
      if(k[1] == 0) {   // if k[1] is small
        for(unsigned w = 0; w < I3.size(); w++) {
          Type u1 = a * I3[w].first + c;
          Type u2 = a * I3[w].second + c;

          // BEGIN pre evalution of power of all
          std::vector <Type> diff_u_pow(m + 2 * s + 2, 0) ;
          Type u1pi = u1;
          Type u2pi = u2;
          for(unsigned pwr = 1; pwr <= m + 2 * s + 1 ; pwr++, u1pi *= u1, u2pi *= u2) {
            //               diff_u_pow[pwr] = (pow(u2, pwr) - pow(u1, pwr)) / (pwr) ;
            diff_u_pow[pwr] = (u2pi - u1pi) / (pwr) ; // TODO TOCHECK
          }
          std::vector <Type> pow_c(m + 1, 0) ;
          pow_c[0] = 1;
          for(unsigned pwr = 1; pwr <= m ; pwr++) {
            pow_c[pwr] = (-c) * pow_c[pwr - 1] ;
          }
          std::vector <Type> pow_k0(s + 1, 0) ;
          std::vector <Type> pow_k2(s + 1, 0);
          pow_k0[0] = 1;
          pow_k2[0] = 1;
          for(unsigned pwr = 1; pwr <= s ; pwr++) {
            pow_k0[pwr] = k[0] * pow_k0[pwr - 1] ;
            pow_k2[pwr] = k[0] * pow_k2[pwr - 1] ;
          }
          // END pre evalution of power of all

          //         k[2] = pol1[2] - pol1[1]*c /(2*a);
          for(int p = 0; p <= m; p++) {
            Type sum(0);
            for(int q = 0; q <= qMax; q++) {
              int pwr = 2 * q + i + p + 1 ;
              sum += pow_k2[qMax - q] * pow_k0[q] * diff_u_pow[pwr] / (factorial<Type>(q) * factorial<Type>(qMax - q))  ;
            }
            A3 += sum * pow_c[m - p] / (factorial<Type>(p) * factorial<Type>(m - p)) ;
          }
        }
      }

      else { // main integral
        // BEGIN pre evaluation A[q] and B[q]
        std::vector <Type> A(s - i + 1, 0);  // size of all this vector changes.
        std::vector <Type> B(s - i + 1, 0);
        if(k[1] != 0) {
          Type kterms = (k[0] * k[2]) / (k[1] * k[1]);
          for(int q = 0; q <= qMax; q++) {
            Type term(1);
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
            //           B[q] = A[q] * (pow(k[1], q) * pow(k[0], qMax - q)) / (factorial<Type>(q) * factorial<Type>(qMax - q));
            //           A[q] *= (pow(k[1], q) * pow(k[2], qMax - q)) / (factorial<Type>(q) * factorial<Type>(qMax - q));

            A[q] *= pow(k[1], q) / (factorial<Type>(q) * factorial<Type>(qMax - q));
            B[q] = A[q] * pow(k[0], qMax - q);
            A[q] *= pow(k[2], qMax - q);

            //         std::cout<<"A["<<q<<"] = " << A[q] <<"  B[] ="<< B[q] << std::endl;
            //         std::cout << "A[" << q << "] = " << A[q] << "  B[] =" << B[q] << std::endl;
          }
        }
        // END  pre evaluation

        for(unsigned w = 0; w < I3.size(); w++) {
          Type u1 = a * I3[w].first + c;
          Type u2 = a * I3[w].second + c;

          // BEGIN pre evalution of power of U
          std::vector <Type> diff_u_pow(m + 2 * s + 2, 0) ;
          Type u1pi = u1;
          Type u2pi = u2;
          for(unsigned pwr = 1; pwr <= m + 2 * s + 1 ; pwr++, u1pi *= u1, u2pi *= u2) {
            //               diff_u_pow[pwr] = (pow(u2, pwr) - pow(u1, pwr)) / (pwr) ;
            diff_u_pow[pwr] = (u2pi - u1pi) / (pwr) ;
          }
          // END
          // BEGIN pre evalution of power of -c
          std::vector <Type> pow_c(m + 1, 0) ;
          pow_c[0] = 1;
          for(unsigned pwr = 1; pwr <= m ; pwr++) {
            pow_c[pwr] = (-c) * pow_c[pwr - 1] ;
          }
          // END pre evalution of power of -c

          //Type A3i(0);
          for(unsigned p = 0; p <= m; p++) {
            Type sum1(0);
            int pwr = p + i + 1;
            for(unsigned q = 0; q <= qMax; q++, pwr++) {
              //             int pwr = p + q + i + 1;
              sum1 += A[q] * diff_u_pow[pwr];
            }
            Type sum2(0);
            pwr = 2 * s - i + p + 1;
            for(unsigned q = 0; q < qMax; q++, pwr--) {
              //int pwr = 2 * s - i + p - q + 1;
              sum2 += B[q] * diff_u_pow[pwr];
            }
            A3 += (sum1 + sum2) * pow_c[m - p] / (factorial<Type>(p) * factorial<Type>(m - p));
          }
        }
      }
      A3 /= ((n + i + 1) * factorial<Type>(i)) ;
    }
    A3 *= factorial<Type>(m) / pow(a, m + 1);
    //     std::cout<< "final. A3= "<< A3 << std::endl;
  }
  return A3;
}

template <class Type>
void get_p1_p2_p3(const int &table, const std::vector<double> &corner, PointT <Type> &p1, PointT <Type> &p2, PointT <Type> &p3){
    double epsilon = 0.000000000000001;
    Type i1_pm_eps(-1) , i2_pm_eps(-1);

    // std::cout << "Corner " << i << ": (" << corner[0] << ", " << corner[1] << ", " << corner[2] << ") - Print Something\n";

    switch (table) {
        case 0:
//             i1_pm_eps = static_cast<Type>(corner[0]);
//             i2_pm_eps = static_cast<Type>(corner[1]);
//             i2_pm_eps = (corner[1] < epsilon)? static_cast<Type>(epsilon) : static_cast<Type>(corner[1]);

            i1_pm_eps = static_cast<Type>(corner[0] - epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] + epsilon);
//                    if (i1 == partition ) i1_pm_eps = static_cast<Type>(i1*del_x - epsilon);     //it keeps my i2 in (0,1)
            p1 = {static_cast<Type>(0), i1_pm_eps};
            p2 = {i2_pm_eps, static_cast<Type>(1)};
            break;
        case 1:
            //Do we really need epsilon on this table?
            i1_pm_eps = static_cast<Type>(corner[0] + epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] - epsilon);
            p1 = {static_cast<Type>(0), i1_pm_eps};
            p2 = {static_cast<Type>(1), i2_pm_eps};
            break;
        case 2:
            i1_pm_eps = static_cast<Type>(corner[0] + epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] + epsilon);
            p1 = {static_cast<Type>(0), i1_pm_eps};
            p2 = {i2_pm_eps, static_cast<Type>(0)};
            break;
        case 3:
            i1_pm_eps = static_cast<Type>(corner[0] - epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] + epsilon);
            p1 = {i1_pm_eps,static_cast<Type>(1)};
            p2 = {i2_pm_eps, static_cast<Type>(1)};
            break;
        case 4:
            i1_pm_eps = static_cast<Type>(corner[0] - epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] - epsilon);
            p1 = {i1_pm_eps, static_cast<Type>(1)};
            p2 = {static_cast<Type>(1), i2_pm_eps};
            break;
        case 5:
            i1_pm_eps = static_cast<Type>(corner[0] + epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] - epsilon);
            p1 = {i1_pm_eps,static_cast<Type>(1)};
            p2 = {i2_pm_eps, static_cast<Type>(0)};
            break;
        case 6:
            i1_pm_eps = static_cast<Type>(corner[0] + epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] - epsilon);
            p1 = {static_cast<Type>(1), i1_pm_eps};
            p2 = {i2_pm_eps, static_cast<Type>(0)};
            break;
        case 7:
            i1_pm_eps = static_cast<Type>(corner[0] - epsilon);
            i2_pm_eps = static_cast<Type>(corner[1] + epsilon);
            p1 = {i1_pm_eps, static_cast<Type>(0)};
            p2 = {i2_pm_eps, static_cast<Type>(0)};
            break;
    }

    p3 = {(p1.x + p2.x)*0.5 , static_cast<Type>(corner[2])};
    if (fabs(corner[2] - 0) < epsilon ) p3.y = static_cast<Type>(epsilon);
    if (fabs(corner[2] - 1) < epsilon ) p3.y = static_cast<Type>(1-epsilon);


}

template <class Type>
std::vector<Point<Type>> findGridIntersectionPoints(Circle<Type> circle, Point<Type> lineStart, Point<Type> lineEnd , std::vector<int> &ltrbNumber , unsigned int &table_number ) {


    table_number =9999;
    Type epsilon = 0.000000000000001; // put it in all the inequality.

    std::vector<Point<Type>> grid_intersections(0);
    // Circle equation: (x - h)^2 + (y - k)^2 = r^2
    Type h = circle.center.x;
    Type k = circle.center.y;
    Type r = circle.radius;


    Type diff_of_sqr , sqrt_diff_of_sqrt , value_p , value_m ;


    //x = i (Left)and circle
    diff_of_sqr = r*r - (lineStart.x - h)*(lineStart.x - h);
//     cout << " diff square = " << diff_of_sqr << endl;
    if (diff_of_sqr > epsilon ){
       sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
       value_p = k + sqrt_diff_of_sqrt;
       value_m = k - sqrt_diff_of_sqrt;
//        cout << value_m << " -- " << lineStart.y << " " << lineEnd.y ;
      if (value_p + epsilon >= lineStart.y && value_p <= lineEnd.y + epsilon){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {lineStart.x,value_p};
        ltrbNumber[0] += 1;// it means the circle intersected the left.
      }
      if (value_m + epsilon >= lineStart.y && value_m <= lineEnd.y+ epsilon){
//         cout << " !# ";
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {lineStart.x, value_m};
        ltrbNumber[0] += 1;
      }
    }
    else if (fabs(diff_of_sqr - 0) <= epsilon){
        if (k + epsilon >= lineStart.y && k <= lineEnd.y + epsilon){
          grid_intersections.resize(grid_intersections.size()+1);
          grid_intersections[grid_intersections.size()-1] = {lineStart.x,k};
          ltrbNumber[0] += 1;  // it means the circle intersected the left.
        }
    }

    //y = j+1 (Top) and circle  corner points won't register here because of strickly less than.
    diff_of_sqr = r*r - (lineEnd.y - k)*(lineEnd.y - k);
//     cout << " diff square = " << diff_of_sqr << endl;
    if (diff_of_sqr > epsilon ){
       sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
       value_p = h + sqrt_diff_of_sqrt;
       value_m = h - sqrt_diff_of_sqrt;
      if (value_p > lineStart.x + epsilon && value_p + epsilon < lineEnd.x){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {value_p, lineEnd.y};
        ltrbNumber[1] += 1;
      }
      if (value_m > lineStart.x + epsilon && value_m + epsilon < lineEnd.x){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {value_m, lineEnd.y};
        ltrbNumber[1] += 1;
//         cout << " use negative value in top. value m = " << value_m << "diff "<< value_m - lineEnd.x <<endl;
      }
    }
      else if (fabs(diff_of_sqr - 0) <= epsilon){
        if (h > lineStart.x + epsilon && h + epsilon < lineEnd.x){
          grid_intersections.resize(grid_intersections.size()+1);
          grid_intersections[grid_intersections.size()-1] = {h,lineEnd.y};
          ltrbNumber[1] += 1;  // it means the circle intersected the left.
        }
    }

    //x = i+1 (Right) and circle
    diff_of_sqr = r*r - (lineEnd.x - h)*(lineEnd.x - h);
//     cout << " diff square = " << diff_of_sqr << endl;
    if (diff_of_sqr > epsilon ){
       sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
       value_p = k + sqrt_diff_of_sqrt;
       value_m = k - sqrt_diff_of_sqrt;
//        cout << value_m << " -- " <<value_p<< " " <<  lineStart.y << " " << lineEnd.y ;
      if (value_p + epsilon>=lineStart.y && value_p <= lineEnd.y+ epsilon){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {lineEnd.x, value_p};
        ltrbNumber[2] += 1;
      }
      if (value_m + epsilon>=lineStart.y && value_m <= lineEnd.y+ epsilon){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {lineEnd.x, value_m};
        ltrbNumber[2] += 1;
      }
    }
    else if (fabs(diff_of_sqr - 0) <= epsilon){
        if (k + epsilon >=lineStart.y && k <= lineEnd.y+ epsilon){
          grid_intersections.resize(grid_intersections.size()+1);
          grid_intersections[grid_intersections.size()-1] = {lineEnd.x,k};
          ltrbNumber[2] += 1;  // it means the circle intersected the left.
        }
    }

    //y = j (bottom) and circle
    diff_of_sqr = r*r - (lineStart.y - k)*(lineStart.y - k);
//     cout << " diff square = " << diff_of_sqr << endl;
    if (diff_of_sqr > epsilon ){
       sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
       value_p = h + sqrt_diff_of_sqrt;
       value_m = h - sqrt_diff_of_sqrt;
      if (value_p > lineStart.x + epsilon && value_p + epsilon < lineEnd.x){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {value_p, lineStart.y};
        ltrbNumber[3] += 1;
      }
      if (value_m > lineStart.x + epsilon && value_m + epsilon < lineEnd.x){
        grid_intersections.resize(grid_intersections.size()+1);
        grid_intersections[grid_intersections.size()-1] = {value_m, lineStart.y};
        ltrbNumber[3] += 1;
      }
    }
    else if (fabs(diff_of_sqr - 0) <= epsilon){
        if (h > lineStart.x+epsilon && h+ epsilon < lineEnd.x){
          grid_intersections.resize(grid_intersections.size()+1);
          grid_intersections[grid_intersections.size()-1] = {h,lineStart.y};
          ltrbNumber[3] += 1;  // it means the circle intersected the left.
        }
    }



    //TODO check if we have any duplicate points (corner points)
/*
    for(int ia = 0 ; ia < grid_intersections.size() ; ia++){
      for(int ib = ia+1 ; ib < grid_intersections.size() ; ib++){
        if(grid_intersections[ia].x - grid_intersections[ib].x < 0.0000000001){
          if(grid_intersections[ia].y - grid_intersections[ib].y<0.0000000001)
          grid_intersections.erase(grid_intersections.begin()+ib);
        }
      }
    }*/

    //OR we can only check the corner points and take one value in determinant values in sqrt.


    // TODO if we have horizontal parabola we switch x and y .


    if (grid_intersections.size() == 1){ // it means it touches the corner never enters.
      grid_intersections.resize(0);
      cout << " one intersection deleting. \n " ;
    }

    else if(grid_intersections.size() == 3){ // TODO we have to get rid of one corner.
     cout << "######### get rid of a corner ########## -----------------------------------------------------------------------------------------------" <<endl;
    }

    else if(grid_intersections.size() == 2){

//       cout << "it intersect in 2 points.\n ";
      grid_intersections.resize(3);
      grid_intersections[2] = { (grid_intersections[0].x + grid_intersections[1].x)*0.5 , -1.0};
      diff_of_sqr = r*r - ((grid_intersections[2].x - h)*(grid_intersections[2].x - h));
      if (diff_of_sqr >= 0 ){ //it is different for table 5
        sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
        value_p = k + sqrt_diff_of_sqrt;
        value_m = k - sqrt_diff_of_sqrt;
        if (value_p >=lineStart.y && value_p <= lineEnd.y){
          grid_intersections[2].y = value_p;
        }
        else if (value_m >=lineStart.y && value_m <= lineEnd.y ){
          grid_intersections[2].y = value_m;
        }
//         else {
//           grid_intersections.resize(0);
//           ltrbNumber={0,0,0,0};
//           // meaning we have two intersection but mid_point is outside. it is either 4 intersection or never inters}
//         }
      }

//           cout << "LTRB = " << ltrbNumber[0] << ltrbNumber[1]<<ltrbNumber[2]<<ltrbNumber[3] <<endl;

      if (ltrbNumber[0] == 1){
        if (ltrbNumber[1] == 1) {
          table_number = 0;
        }
        else if(ltrbNumber[2] == 1){
          table_number = 1;
        }
        else if(ltrbNumber[3] == 1){
          table_number = 2;
        }
      }

      else if (ltrbNumber[1] == 2) table_number = 3;
      else if (ltrbNumber[1] == 1){
        if(ltrbNumber[2] == 1) table_number = 4;
        else if(ltrbNumber[3] == 1) table_number = 5;
      }
      else if (ltrbNumber[2] == 1 && ltrbNumber[3] == 1) table_number = 6;
      else if (ltrbNumber[3] == 2) table_number = 7;

//       swapping x and y when we have Top - Bottom (table-5) into table-1
      if (grid_intersections.size() == 3){
        cout<< " ("<<grid_intersections[0].x<<"," << grid_intersections[0].y<<") " << " ("<<grid_intersections[1].x<<"," << grid_intersections[1].y<<") " << " ("<<grid_intersections[2].x<<"," << grid_intersections[2].y<<") " <<endl;
      }

      if (table_number == 5){
        grid_intersections[2] = { -1.0, (grid_intersections[0].y + grid_intersections[1].y)*0.5 };
        diff_of_sqr = r*r - ((grid_intersections[2].y - k)*(grid_intersections[2].y - k));
// //         cout << " diff square = " << diff_of_sqr << endl;
        if (diff_of_sqr >= 0 ){
          sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
          value_p = h + sqrt_diff_of_sqrt;
          value_m = h - sqrt_diff_of_sqrt;
// //           cout<< "value_m = "<<value_m << " value_p = "<< value_p <<endl;
          if (value_p >=lineStart.x && value_p <= lineEnd.x){
            grid_intersections[2].x = value_p;
          }
          else if (value_m >=lineStart.x && value_m <= lineEnd.x ){
            grid_intersections[2].x = value_m;
          }
          else {
          grid_intersections.resize(0);
          ltrbNumber={0,0,0,0};
          // meaning we have two intersection but mid_point is outside. it is either 4 intersection or never inters}
          }
        }
      }

    }

//     cout<< " grid intersection size "<<grid_intersections.size() <<endl;
//     for (int gi = 0 ; gi <grid_intersections.size(); gi++){
//       cout<< " ("<<grid_intersections[gi].x<<"," << grid_intersections[gi].y<<") \n" ;
//     }
//     cout << "table = " << table_number <<endl;



//     cout << "table number aa : " << table_number << " " ;

    //introduce middle point if we have two intersections:
//     if (grid_intersections.size() == 2){
//       grid_intersections.resize(3);
//       grid_intersections[2] = { (grid_intersections[0].x + grid_intersections[1].x)/2.0 , -1.0};
//       diff_of_sqr = r*r - ((grid_intersections[2].x - h)*(grid_intersections[2].x - h));
//       if (diff_of_sqr >=0 ){
//         sqrt_diff_of_sqrt = sqrt(diff_of_sqr);
//         value_p = k + sqrt_diff_of_sqrt;
//         value_m = k - sqrt_diff_of_sqrt;
//         if (value_p +epsilon >=lineStart.y && value_p <= lineEnd.y+epsilon){
//           grid_intersections[2].y = value_m;
//         }
//         else if (value_m >=lineStart.y && value_m <= lineEnd.y ){
//           grid_intersections[2].y = value_m;
//         }
//         else grid_intersections.resize(0);  // meaning we have two intersection but mid_point is outside. it is either 4 intersection or never inters}
//       }
//     }

// TODO checkout the Pushback function. It looks nice and easier.
//     if (discriminant >= 0) {
//         // Calculate the intersection points
//         double x1 = (-b + std::sqrt(discriminant)) / (2 * a);
//         double y1 = m * x1 + c;
//         intersections.push_back({x1, y1});
//
//         double x2 = (-b - std::sqrt(discriminant)) / (2 * a);
//         double y2 = m * x2 + c;
//         intersections.push_back({x2, y2});
//     }

    return grid_intersections;
}

template <class Type>
std::vector<Point<Type>> mapIntersectionPoints(std::vector<Point<Type>>grid_intersections, Point<Type> gridStart, Point<Type> gridEnd, unsigned int &table_number , std::vector< Type > &interp_point, bool &isswap){

  std::vector<Point<Type>> mapped_intersections(0);
  interp_point.resize(0);
  for(int aa=0 ; aa < grid_intersections.size(); aa++){
        mapped_intersections.resize(mapped_intersections.size()+1);
        mapped_intersections[mapped_intersections.size()-1] = {((grid_intersections[aa].x - gridStart.x)/(gridEnd.x -gridStart.x)), ((grid_intersections[aa].y - gridStart.y)/(gridEnd.y - gridStart.y))};
        if (mapped_intersections[aa].x < 0) mapped_intersections[aa].x = 0;
        else if (mapped_intersections[aa].x > 1) mapped_intersections[aa].x = 1;
        if (mapped_intersections[aa].y < 0) mapped_intersections[aa].y = 0;
        else if (mapped_intersections[aa].y >1) mapped_intersections[aa].y = 1;   //TODO check if it should be .y
//       mapped_intersections.push_back({((intersections[aa].x - gridStart.x)/(gridEnd.x -gridStart.x)) , ((intersections[aa].y - gridStart.y)/           //       (gridEnd.y -gridStart.y))});

  }
// for(int mk=0;mk<mapped_intersections.size();mk++){
//   cout<< "before swapping : ("<<mapped_intersections[mk].x << " " <<mapped_intersections[mk].y << ") "<<endl;
// }


    if(table_number == 5){
      cout << " swapping values :::::::::" <<endl;
      isswap = 1;
      Type swapvalue;
      swapvalue = mapped_intersections[0].x ;
      mapped_intersections[0].x = mapped_intersections[0].y;
      mapped_intersections[0].y = swapvalue;

      swapvalue = mapped_intersections[1].x ;
      mapped_intersections[1].x = mapped_intersections[1].y;
      mapped_intersections[1].y = swapvalue;

      swapvalue = mapped_intersections[2].x ;
      mapped_intersections[2].x = mapped_intersections[2].y;
      mapped_intersections[2].y = swapvalue;

      //TODO fro direct formula we need to exchange p1 and p2

      swapvalue = mapped_intersections[0].x ;
      mapped_intersections[0].x = mapped_intersections[1].x;
      mapped_intersections[1].x = swapvalue;

      swapvalue = mapped_intersections[0].y ;
      mapped_intersections[0].y = mapped_intersections[1].y;
      mapped_intersections[1].y = swapvalue;



      table_number = 1;
//       for(int mk=0;mk<mapped_intersections.size();mk++){
//         cout<< "after swapping : ("<<mapped_intersections[mk].x << " " <<mapped_intersections[mk].y << ") "<<endl;
//       }
    }

  if(mapped_intersections.size() == 3){ // Finding interpolation points
    switch (table_number) {
                case 0:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].y) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                    break;
                case 1:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].y) , static_cast<Type>(mapped_intersections[1].y) , static_cast<Type>(mapped_intersections[2].y)};
                    break;
                case 2:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].y) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                    break;
                case 3:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].x) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                    break;
                case 4:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].x) , static_cast<Type>(mapped_intersections[1].y) , static_cast<Type>(mapped_intersections[2].y)};
                    break;
                case 5:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].x) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                  break;
                case 6:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].y) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                  break;
                case 7:
                  interp_point.resize(3);
                  interp_point = { static_cast<Type>(mapped_intersections[0].x) , static_cast<Type>(mapped_intersections[1].x) , static_cast<Type>(mapped_intersections[2].y)};
                  break;
    }
  }


  return mapped_intersections;
}

template <class Type>
Type find_area(const unsigned &m, const unsigned &n, const int &s, const Type &a, Type c,const int &table,  PointT <Type> &p1,  PointT <Type> &p2, const PointT <Type> &p3){
    Type area(0);
    Type A1 (0), A2 (0), A3 (0);
    std::vector< std::pair <Type, Type> > I1, I2, I3 ;
    int intersect_number;
    std::vector <Type> pol1(3, 0);
    std::vector <Type> pol2(3, 0);
    std::vector <Type> intersection;
    std::vector <Type> interp_point;    //never used in this function. it was used in interpolation;
    unsigned int table_number = table;
    Parabola <Type> parabola;
    Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x) ;// only sort out the points parallel to y axis
    parabola = get_parabola_equation(p1, p2, p3);
    CheckIntersection <Type> (intersect_number, table_number, intersection, interp_point, parabola);


    Type k = parabola.k;
    Type b = parabola.b;
    Type d = parabola.d;
    if (table == 0){
      if (k>0) {
        Type delta = b*b - 4*k*(d+1);
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x > 0 && x < p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 2){
      if (k<0) {
        Type delta = b*b - 4*k*d;
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x > 0 && x < p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 4){
      if (k>0) {
        Type delta = b*b - 4*k*(d+1);
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x <1 && x > p1.x) {
                    p1.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 6){
      if (k<0) {
        Type delta = b*b - 4*k*d;
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x < 1 && x > p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }
//     cout<< "parabola = " << parabola.k <<"x^2 +"<<parabola.b<<"x +" <<parabola.d << "+y=0 "<<endl;  //     cout<< "intersection = " << intersect_number << " table= " << table_number <<endl;
    k = -parabola.k;
    b = -parabola.b;
    d = -parabola.d;
      if (table == 2){
        area = (1-d - k*p2.x*p2.x/3 - b*p2.x/2)*p2.x - (1-d - k*p1.x*p1.x/3 - b*p1.x/2)*p1.x + 1 -p2.x ;
      }
      else if (table == 6){
        area = (1-d - k*p1.x*p1.x/3 - b*p1.x/2)*p1.x - (1-d - k*p2.x*p2.x/3 - b*p2.x/2)*p2.x + p2.x ;
      }
      else{
        area = (1-d - k*p2.x*p2.x/3 - b*p2.x/2)*p2.x - (1-d - k*p1.x*p1.x/3 - b*p1.x/2)*p1.x;
      }

    return area ;
}

template <class Type>
Type find_area_2intersection_formula(const unsigned &m, const unsigned &n, const int &s, const Type &a, Type c,const int &table,  PointT <Type> &p1,  PointT <Type> &p2, const PointT <Type> &p3){
    Type area(0);
    Type A1 (0), A2 (0), A3 (0);
    std::vector< std::pair <Type, Type> > I1, I2, I3 ;
    int intersect_number;
    std::vector <Type> pol1(3, 0);
    std::vector <Type> pol2(3, 0);
    std::vector <Type> intersection;
    std::vector <Type> interp_point;    //never used in this function. it was used in interpolation;
    unsigned int table_number = table;
    Parabola <Type> parabola;
    Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x) ;// only sort out the points parallel to y axis
    parabola = get_parabola_equation(p1, p2, p3);
    CheckIntersection <Type> (intersect_number, table_number, intersection, interp_point, parabola);

    Type k = parabola.k;
    Type b = parabola.b;
    Type d = parabola.d;

    bool do_line = 0;

    if (table == 0){
      if (k>0) {
        do_line = 1;
        Type delta = b*b - 4*k*(d+1);
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x > 0 && x < p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 2){
      if (k<0) {
                do_line = 1;
        Type delta = b*b - 4*k*d;
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x > 0 && x < p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 4){
      if (k>0) {
                do_line = 1;
        Type delta = b*b - 4*k*(d+1);
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x <1 && x > p1.x) {
                    p1.x = x;
                }
                sign *= -1;
              }
            }
      }
    }

    if (table == 6){
      if (k<0) {
                do_line = 1;
        Type delta = b*b - 4*k*d;
//         cout << " k = "<< k << " b = "<< b << " d ="<< d << " delta = " << delta <<endl;
        if (delta >= 0){
              Type sqrtdelta = sqrt(delta);
              int sign = (k > 0) ? 1 : -1;
              for(unsigned i = 0; i < 2; i++) {
                Type x = (- b - sign * sqrtdelta) / (2 * k);
    //             cout<< "Top x = "<< x<< endl;
                 if(x < 1 && x > p2.x) {
                    p2.x = x;
                }
                sign *= -1;
              }
            }
      }
    }


    pol2[0] = parabola.k; pol2[1] = parabola.b; pol2[2] = parabola.d;
    if(do_line){
      if (table == 0){
        I1.resize(1, std::pair<Type, Type>(static_cast<Type>(0), static_cast<Type>(p2.x)));
        area = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
      }

      else if (table == 2){
          I1.resize(1, std::pair<Type, Type>(static_cast<Type>(0), static_cast<Type>(p2.x)));
          I3.resize(1, std::pair<Type, Type>(static_cast<Type>(p2.x), static_cast<Type>(1)));
          area = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1) + easy_integral_A3(m, n, s, a, c, pol2, I3);
        }

      else if (table == 4){
        I1.resize(1, std::pair<Type, Type>(static_cast<Type>(p1.x), static_cast<Type>(1)));
        area = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
      }

      else if (table == 6){
          I1.resize(1, std::pair<Type, Type>(static_cast<Type>(p2.x), static_cast<Type>(1)));
          I3.resize(1, std::pair<Type, Type>(static_cast<Type>(0), static_cast<Type>(p2.x)));
          area = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1) + easy_integral_A3(m, n, s, a, c, pol2, I3);
        }
    }

     else {

       pol1[0] = parabola.k ; pol1[1] = a + parabola.b; pol1[2] = c + parabola.d;
        GetIntervalall<Type, double>(pol1, pol2, I1, I2, I3);

        if(I1.size() > 0) {
            A1 = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
        }
        if(I2.size() > 0) {
            A2 = easy_integral_A2(m, n, s, a, c, pol2, I2);
        }
        if(I3.size() > 0) {
            A3 = easy_integral_A3(m, n, s, a, c, pol2, I3);
        }
        area = A1 + A2 + A3;
    }


    return area ;
}


template <class Type>
Type trilinier_interpolation(std::vector< std::vector< Type >> & interp_table , const std::vector< Type > &interp_point){

  Type x = interp_point[0];
  Type y = interp_point[1];
  Type z = interp_point[2];

    Type c_000 = interp_table[0][3];
    Type c_001 = interp_table[1][3];
    Type c_010 = interp_table[2][3];
    Type c_011 = interp_table[3][3];
    Type c_100 = interp_table[4][3];
    Type c_101 = interp_table[5][3];
    Type c_110 = interp_table[6][3];
    Type c_111 = interp_table[7][3];

    Type x0 = interp_table[0][0];
    Type x1 = interp_table[7][0];
    Type y0 = interp_table[0][1];
    Type y1 = interp_table[7][1];
    Type z0 = interp_table[0][2];
    Type z1 = interp_table[7][2];

    Type x_d = (x-x0)/(x1-x0);
    Type y_d = (y-y0)/(y1-y0);
    Type z_d = (z-z0)/(z1-z0);

  Type c_00 = c_000 * (1-x_d) + c_100 * x_d ;
  Type c_01 = c_001 * (1-x_d) + c_101 * x_d ;
  Type c_10 = c_010 * (1-x_d) + c_110 * x_d ;
  Type c_11 = c_011 * (1-x_d) + c_111 * x_d ;

  Type c_0 = c_00 * (1-y_d) + c_10 * y_d ;
  Type c_1 = c_01 * (1-y_d) + c_11 * y_d ;

  Type cc = c_0 * (1-z_d) + c_1 * z_d ;

  return cc;
}

template <class Type>
double findGridArea(const Circle<Type> &circle, const Point<Type> &gridStart, const Point<Type> &gridEnd, std::vector<Point<Type>> grid_intersections, std:: vector <std:: vector <double>> interp_table, const std::vector< Type > &interp_point, const Type &grid_size){
  double area = 0;
  if(grid_intersections.size() <= 1){
    // checking wherher the grid is outside or inside the circle . Circle equation: (x - h)^2 + (y - k)^2 = r^2 Find (i+epsilon-h)^2 +(j+epsilon-k)^2 < r^2
    area = ((gridStart.x - circle.center.x )*(gridStart.x - circle.center.x) + (gridStart.y - circle.center.y)*(gridStart.y - circle.center.y) < (circle.radius*circle.radius)) ? 1 : 0 ;
  }
  else if (interp_point.size() == 3){ // here goes our ex_23

      cout << " using interpolation " << endl;
      std::vector< double > double_interp_point = {static_cast<double>(interp_point[0]), static_cast<double>(interp_point[1]),static_cast<double>(interp_point[2])};

      area = trilinier_interpolation<double>(interp_table , double_interp_point) ;


//       cout << "grid area before scaling = "<< area <<endl;
  }

  else{} // direct calculation for 4 intersections. use grid intersections to find the area of for intersection points.

//   cout << "grid size = "<< grid_size <<endl;
  area = area * static_cast<double>(grid_size) * static_cast<double>(grid_size) ;
//   cout << "grid area after scaling = "<< area <<endl;

  return area;


}

struct Point3D {
    double x, y, z;
    Point3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
};

// Define an Octree Node
template <class Type>
class OctreeNode {
public:
    Point3D minBounds, maxBounds;
    bool isLeaf;
    std::vector<OctreeNode*> children;
    std::vector<std::vector<double>> corners;
    int table;
    unsigned int m = 0;
    unsigned int n = 0;
    unsigned int mPn = 2;
    int s = 0;
    unsigned depth = 0;
    Type a = 0;
    double relative_error = -1;
    double relative_error_opposite = -1;

OctreeNode(const Point3D& _minBounds, const Point3D& _maxBounds, const int& _table, const int& _depth )
        : minBounds(_minBounds), maxBounds(_maxBounds), isLeaf(true), table(_table), depth(_depth) {}

    // Function to get the eight corners of the node
    void getCorners() {
            corners.push_back({minBounds.x, minBounds.y, minBounds.z});
            corners.push_back({minBounds.x, minBounds.y, maxBounds.z});
            corners.push_back({minBounds.x, maxBounds.y, minBounds.z});
            corners.push_back({minBounds.x, maxBounds.y, maxBounds.z});
            corners.push_back({maxBounds.x, minBounds.y, minBounds.z});
            corners.push_back({maxBounds.x, minBounds.y, maxBounds.z});
            corners.push_back({maxBounds.x, maxBounds.y, minBounds.z});
            corners.push_back({maxBounds.x, maxBounds.y, maxBounds.z});

//         for (const auto& corner : corners) {
//             std::cout << "Corner: (" << corner[0] << ", " << corner[1] << ", " << corner[2] << ") - Print Something\n";
//         }
            Type area(0) ;
            Type c(1) ;
            std::vector<double> integral_m_n;
            PointT <Type> p1, p2, p3 ;

        for (size_t i = 0; i < corners.size(); ++i) {
            const auto& corner = corners[i];
//             std::cout << "Corner " << i << ": (" << corner[0] << ", " << corner[1] << ", " << corner[2] << ") - Print Something\n";searchPoint
            get_p1_p2_p3(table, corner, p1, p2, p3);


//             area = find_area_2intersection_formula(m,n,s,a,c,table, p1,p2, p3);
//             corners[i][3] = static_cast<double>(area);

            for (size_t ii = 0; ii <= mPn; ++ii){
              for (size_t jj = 0; ii+jj <= mPn; ++jj){
                area = find_area_2intersection_formula(jj,ii,s,a,c,table, p1,p2, p3);
                corners[i].push_back(static_cast<double>(area));
              }
            }
        }
      }



      void subdivideWithRelativeError(int maxDepth, double maxRelativeError, int currentDepth = 0) {
        if (currentDepth >= maxDepth || !isLeaf) {
        getCorners();
        return;
        }

        getCorners();

        double midX = (minBounds.x + maxBounds.x) / 2.0;
        double midY = (minBounds.y + maxBounds.y) / 2.0;
        double midZ = (minBounds.z + maxBounds.z) / 2.0;
        std::vector<double> relativeErrors;
        std::vector<double> relativeErrorsOpposite;
        std::vector<std::vector<double>>boundary_mid_points(19);

        boundary_mid_points[0]={midX, minBounds.y, minBounds.z};
        boundary_mid_points[1]={maxBounds.x, midY, minBounds.z};
        boundary_mid_points[2]={midX, maxBounds.y, minBounds.z};
        boundary_mid_points[3]={minBounds.x,midY,minBounds.z};
        boundary_mid_points[4]={minBounds.x,minBounds.y,midZ};
        boundary_mid_points[5]={maxBounds.x,minBounds.y,midZ};
        boundary_mid_points[6]={maxBounds.x,maxBounds.y,midZ};
        boundary_mid_points[7]={minBounds.x,maxBounds.y,midZ};
        boundary_mid_points[8]={midX, minBounds.y, maxBounds.z};
        boundary_mid_points[9]={maxBounds.x, midY, maxBounds.z};
        boundary_mid_points[10]={midX, maxBounds.y, maxBounds.z};
        boundary_mid_points[11]={minBounds.x,midY,maxBounds.z};
        boundary_mid_points[12]={midX,midY,minBounds.z};
        boundary_mid_points[13]={minBounds.x,midY,midZ};
        boundary_mid_points[14]={midX,minBounds.y,midZ};
        boundary_mid_points[15]={midX,midY,maxBounds.z};
        boundary_mid_points[16]={maxBounds.x,midY,midZ};
        boundary_mid_points[17]={midX,maxBounds.y,midZ};
        boundary_mid_points[18]={midX,midY,midZ};

        for (int i = 0; i < boundary_mid_points.size(); ++i) { //TODO all nineteen points
            // Generate a random 3D point within the bounds
//             double randomX = minBounds.x + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (maxBounds.x - minBounds.x);
//             double randomY = minBounds.y + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (maxBounds.y - minBounds.y);
//             double randomZ = minBounds.z + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (maxBounds.z - minBounds.z);

            //not random using middle boundary points
            double randomX = boundary_mid_points[i][0];
            double randomY = boundary_mid_points[i][1];
            double randomZ = boundary_mid_points[i][2];

            std::vector<double> interp_point = {randomX, randomY, randomZ};


            Type f_area(0);
            Type c = 1 ;
            PointT<Type> p1, p2, p3;
            get_p1_p2_p3(table, interp_point, p1, p2, p3);

            std::vector<std::vector<double>>interpolation_vector(8);

            for (size_t ii = 0; ii <= mPn; ++ii){
              for (size_t jj = 0; ii+jj <= mPn; ++jj){

                size_t index = 0;
                for (size_t mm = 0; mm < ii; ++mm){
                  index += mPn + 1 - mm ;
                }

                index += jj+3 ;

                for (size_t ic = 0; ic < corners.size(); ++ic) {
                  interpolation_vector[ic] = {corners[ic][0],corners[ic][1],corners[ic][2],corners[ic][index]};
                }



                double interp_area = trilinier_interpolation(interpolation_vector, interp_point);
                f_area = find_area_2intersection_formula(jj,ii,s,a,c,table, p1,p2, p3);
                double formula_area = static_cast<double>(f_area);
                double r_error = fabs(formula_area - interp_area) / formula_area;
                double r_error_opposite = fabs(formula_area - interp_area) /(1.0/(m+1)*(n+1) - formula_area);

                relativeErrors.push_back(r_error);
                relativeErrorsOpposite.push_back(r_error_opposite);
              }
            }


//                 f_area = find_area_2intersection_formula(m, n, s, a, c, table, p1, p2, p3);
//                 double formula_area = static_cast<double>(f_area);

    //             double r_error = (m+1)*(n+1) * fabs(formula_area - interp_area) / formula_area;
    //             double r_error_opposite = fabs(formula_area - interp_area) /(1.0/(m+1)*(n+1) - formula_area);

//                 double r_error = fabs(formula_area - interp_area) / formula_area;
//                 double r_error_opposite = fabs(formula_area - interp_area) /(1.0/(m+1)*(n+1) - formula_area);
//
//                 relativeErrors.push_back(r_error);
//                 relativeErrorsOpposite.push_back(r_error_opposite);



        }
        relative_error = *std::max_element(relativeErrors.begin(), relativeErrors.end());
        relative_error_opposite = *std::max_element(relativeErrorsOpposite.begin(), relativeErrorsOpposite.end());



//         std::vector< double > interp_point = {midX,midY,midZ};
//         double interp_area = trilinier_interpolation(corners , interp_point);

//         cout <<  interp_point[0]  << " " << interp_point[1] << " " << interp_point[2] << " "<<endl ;

//         Type f_area(0) ;
//         Type c = 1 ;
//         PointT <Type> p1, p2, p3 ;
//         get_p1_p2_p3(table, interp_point, p1, p2, p3);
//         f_area = find_area(m,n,s,a,c,table, p1,p2, p3);
//         double formula_area = static_cast<double>(f_area);
//         relative_error = fabs(formula_area - interp_area)/formula_area;


//         cout << " interp_area = " << interp_area << " f_area =" << f_area << " r error = "<< relative_error << endl;

        if (depth <= 3 || relative_error > maxRelativeError || relative_error_opposite > maxRelativeError || minBounds.y < 0.003 ) {
            isLeaf = false;
            children.push_back(new OctreeNode(minBounds, {midX, midY, midZ}, table, depth + 1));
            children.push_back(new OctreeNode({midX, minBounds.y, minBounds.z}, {maxBounds.x, midY, midZ}, table, depth + 1));
            children.push_back(new OctreeNode({minBounds.x, midY, minBounds.z}, {midX, maxBounds.y, midZ}, table, depth + 1));
            children.push_back(new OctreeNode({midX, midY, minBounds.z}, {maxBounds.x, maxBounds.y, midZ}, table, depth + 1));
            children.push_back(new OctreeNode({minBounds.x, minBounds.y, midZ}, {midX, midY, maxBounds.z}, table, depth + 1));
            children.push_back(new OctreeNode({midX, minBounds.y, midZ}, {maxBounds.x, midY, maxBounds.z}, table, depth + 1));
            children.push_back(new OctreeNode({minBounds.x, midY, midZ}, {midX, maxBounds.y, maxBounds.z}, table, depth + 1));
            children.push_back(new OctreeNode({midX, midY, midZ}, maxBounds, table, depth + 1));

            // Recursively check for subdivision in each child node
            for (OctreeNode* child : children) {
                child->subdivideWithRelativeError(maxDepth, maxRelativeError, currentDepth + 1);
            }
        }
    }
    // Recursive search for the smallest sub-cube containing a given point
    OctreeNode* search(const Point3D& point) {
        if (isLeaf) {
            return this;
        }

        for (OctreeNode* child : children) {
            if (point.x >= child->minBounds.x && point.x <= child->maxBounds.x &&
                point.y >= child->minBounds.y && point.y <= child->maxBounds.y &&
                point.z >= child->minBounds.z && point.z <= child->maxBounds.z) {
                return child->search(point);
            }
        }

        return nullptr; // Should not reach here under normal circumstances
    }
};



template <class Type>
void printOctreeStructure(OctreeNode<Type>* node, int depth = 0) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    std::cout << "Node Bounds: ";
    std::cout << "(" << node->minBounds.x << ", " << node->minBounds.y << ", " << node->minBounds.z << ") to ";
    std::cout << "(" << node->maxBounds.x << ", " << node->maxBounds.y << ", " << node->maxBounds.z << ")";

    if (node->isLeaf) {
        std::cout << " relative error = " << node-> relative_error <<" " <<node->depth << " [Leaf] \n";

//         Print the corner vectors for the leaf node
        std::cout << "  Corners:\n";
      for (const auto& corner : node->corners) {

          for (const auto& entry : corner) {
              std::cout << " " << entry;
          }
          std::cout << std::endl;
      }
    }
    else {
        std::cout<< " relative error = " << node-> relative_error <<" "<< node->depth << " [Non-Leaf]\n";
        for (OctreeNode<Type>* child : node->children) {
            printOctreeStructure(child, depth + 1);
        }
    }
}







int main() {
    typedef cpp_bin_float_oct Type;      //     typedef double Type;
//     std::cout.precision(16);

    srand(10); // Fixed seed for random number generation
    std::vector<std::vector<double>> pointsVector;
    unsigned int m = 0;
    unsigned int n = 0;
    int s = 0;
    int table = 0;
    Type a(0);
    Type c (1) ;
    int maxDepth = 5;
    std::vector<OctreeNode<Type>> roots;

    for (int ttable = 0; ttable < 8; ++ttable) {
        OctreeNode<Type> root({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0},ttable, 0);
        if(ttable == 0 || ttable == 1 || ttable == 2 || ttable == 4 || ttable == 6){
          root.subdivideWithRelativeError(maxDepth, 0.01);
        }
        else {
          root.subdivideWithRelativeError(3, 0.1);
        }

        std::cout << "Octree Structure:\n";
        roots.push_back(root);
    }

//     printOctreeStructure(&roots[0]);
//     return 0;



















































//         table = 6;
//         Point3D searchPoint(0.2, 0.8775, 0.08888);
//         table = 2;
//         Point3D searchPoint(0.2, 0.2, 0.07);
//
//         OctreeNode<Type>* result = roots[table].search(searchPoint);
//         // Display the result
//         cout<< "table = "<<table << endl;
//         std::cout << "\nSearch Point: (" << searchPoint.x << ", " << searchPoint.y << ", " << searchPoint.z << ")\n";
//         std::cout << "Smallest Sub-cube Bounds: ";
//         std::cout << "(" << result->minBounds.x << ", " << result->minBounds.y << ", " << result->minBounds.z << ") to ";
//         std::cout << "(" << result->maxBounds.x << ", " << result->maxBounds.y << ", " << result->maxBounds.z << ")\n";
//         std::cout << "depth : = " << result->depth << " \n";
//         std::vector<std::vector<double>> interp_table;
//         for (size_t i = 0; i < result->corners.size(); ++i) {
//             interp_table.push_back(result->corners[i]);
//         }
//         std::cout << "Interp Table:\n";
//         for (const auto& corner : interp_table) {
//             std::cout << "(" << corner[0] << ", " << corner[1] << ", " << corner[2] << ", "<< corner[3] <<")\n";
//         }
//
//
//         std::vector<double>interp_point ={searchPoint.x,searchPoint.y,searchPoint.z};
//         std::cout << "\n interp Point: (" << interp_point[0] << ", " << interp_point[1] << ", " << interp_point[2] << ")\n";
//
//         double interp_area = trilinier_interpolation(interp_table , interp_point);
//         Type f_area(0) ;
//         PointT <Type> p1, p2, p3 ;
//         get_p1_p2_p3(table, interp_point, p1, p2, p3);
//         cout << "points = (" << p1.x << ", "<<p1.y << "), ( "<< p2.x<<","<<p2.y<<"),("<<p3.x<<","<<p3.y<<")" << endl;
//         f_area = find_area(m,n,s,a,c,table, p1,p2, p3);
//         double formula_area = static_cast<double>(f_area);
//         double relative_error = fabs(formula_area - interp_area)/formula_area;
//         cout << " interp_area = " << interp_area << " f_area =" << formula_area << " r error = "<< relative_error << endl;
//         Parabola <Type> parabola = get_parabola_equation(p1, p2, p3);
//         cout<< "parbola = " << parabola.k <<"x^2 + "<<parabola.b << "x+" << parabola.d <<"+y =0" <<endl;
//
//         f_area = find_area_2intersection_formula(m,n,s,a,c,table, p1,p2, p3);
//         cout << " using 2 intersection formula = "<< f_area <<endl;
//
// //         return 0;
//
//       {
//         std::cout << "on corner\n";
//         for (const auto& corner : interp_table) {
//             std::cout << "\n (" << corner[0] << ", " << corner[1] << ", " << corner[2] << ", "<< corner[3] <<")\n";
//             get_p1_p2_p3(table, corner, p1, p2, p3);
//             std::cout << "("<<p1.x<<","<< p1.y <<"),(" <<p2.x<<","<< p2.y <<"),("<<p3.x<<","<< p3.y <<")"<<endl;
// //             Parabola<Type> parabola = get_parabola_equation(p1,p2,p3);
// //             cout<< "parbola = " << parabola.k <<"x^2 + "<<parabola.b << "x+" << parabola.d <<"+y =0" <<endl;
// //             f_area = find_area(m,n,s,a,c,table, p1,p2, p3);
// //             cout << "area =" << f_area <<endl;
//
//             Type area(0);
//             Type A1 (0), A2 (0), A3 (0);
//             std::vector< std::pair <Type, Type> > I1, I2, I3 ;
//             int intersect_number;
//             std::vector <Type> pol1(3, 0);
//             std::vector <Type> pol2(3, 0);
//             std::vector <Type> intersection;
//             std::vector <Type> interp_point;    //never used in this function. it was used in interpolation;
//             unsigned int table_number = table;
//             Parabola <Type> parabola;
//             Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x) ;// only sort out the points parallel to y axis
//             parabola = get_parabola_equation(p1, p2, p3);
//             cout<< "parbola = " << parabola.k <<"x^2 + "<<parabola.b << "x+" << parabola.d <<"+y =0" <<endl;
//             CheckIntersection <Type> (intersect_number, table_number, intersection, interp_point, parabola);
//
//             cout<< "number of intersection " << intersect_number << " table = "<< table << /*" interp_point = " << interp_point[0] << " "<< interp_point[1]<<" "<<interp_point[2]*/endl;
//
//             bool concaveUp = 0;
//             bool do_line = 0;
//             if(parabola.k < 0) concaveUp = 1; //kx^2+bx+d+cy = 0 ; c is initialy 1 => y = -kx^2 - bx - d .
//
//             if (intersect_number > 2){
//                 if(!concaveUp){
//                     if (table == 0 /*|| table == 1*/ || table == 4 || table == 5){
//                         do_line = 1 ;
//                     }
//                 }
//                 else {
//                     if (table == 2 || table == 5 || table == 6 ){
//                         do_line = 1 ;
//                     }
//
//                 }
//             }
//
//             cout << "concaveUp " << concaveUp << " do line = " << do_line << endl;
//
//             if (intersect_number == 2 || do_line == 0){ // we are not changing 4 intersection when do table-1 left-right
//                 if (det !=0){
//                     pol1[0] = parabola.k;
//                     pol1[1] = parabola.b;
//                     pol1[2] = parabola.d + c;
//                     pol2[0] = parabola.k;
//                     pol2[1] = parabola.b;
//                     pol2[2] = parabola.d;
//                 }
//                 else {  //TODO decode what I did here. Couldn't figure it out what I did here. We are not using this probably after introducing epsilon
//                   //TODO the area should be 0 or 1 fix it
//
//                   cout<< "this is a straight line. switch " <<endl;
// //                     pol1[0] = static_cast<Type>(0);
// //                     pol1[1] = static_cast<Type>(-1);
// //                     pol1[2] = p1.x;
// //                     pol2[0] = pol1[0];
// //                     pol2[1] = pol1[1];
// //                     pol2[2] = p1.x;
// //                     c=static_cast<Type>(0);
//
//                 }
//
//
//
//               cout<< "pol2= "<< pol2[0] << "x^2 + "<< pol2[1] <<"x + "<< pol2[2] << " + y = 0 ;"<< endl;
//
//               GetIntervalall<Type, double>(pol1, pol2, I1, I2, I3);
//
//               if(I1.size() > 0) {
//                   A1 = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
//               }
//               if(I2.size() > 0) {
//                   A2 = easy_integral_A2(m, n, s, a, c, pol2, I2);
//               }
//               if(I3.size() > 0) {
//                   A3 = easy_integral_A3(m, n, s, a, c, pol2, I3);
//               }
//
//               cout << " A1 = " << A1 << " A2 = " << A2 << " A3 = " << A3 <<endl;
//               area = A1 + A2 + A3;
//
//               cout <<" area = "<< area<<endl;
//
//
//             }
//             else if (do_line == 1){
// //                 if(table !=4){
// //                   Type slope = (p3.y-p1.y)/(p3.x-p1.x);
// //                   c=static_cast<Type>(1);
// //                   pol2[0] = static_cast<Type>(0);    //k=0
// //                   pol2[1] = -slope;
// //                   pol2[2] = slope*p1.x - p1.y ;
// //                   pol1[0] = pol2[0];    //k=0
// //                   pol1[1] = pol2[1];
// //                   pol1[2] = pol2[2] + c ;
// //                 }
// //                 else{ //in table 4 it is between p2 and p3
// //                     Type slope = (p3.y-p2.y)/(p3.x-p2.x);
// //                     pol2[0] = static_cast<Type>(0);    //k=0
// //                     pol2[1] = -slope;
// //                     pol2[2] = slope*p2.x - p2.y ;
// //                     pol1[0] = pol2[0];    //k=0
// //                     pol1[1] = pol2[1];
// //                     pol1[2] = pol2[2] + c ;
// //                 }
// //                 Type slope = (p2.y-p1.y)/(p2.x-p1.x);
// //                 c=static_cast<Type>(1);
// //                 pol2[0] = static_cast<Type>(0);    //k=0
// //                 pol2[1] = -slope;
// //                 pol2[2] = -(p1.y + p3.y - slope*(p1.x + p3.x)) * static_cast<Type>(0.5) ;
// //                 pol1[0] = pol2[0];    //k=0
// //                 pol1[1] = pol2[1];
// //                 pol1[2] = pol2[2] + c ;
//
//                 if(table == 0){
//                   PointT<Type>q1 = {p1.x,p1.y};
//                   PointT<Type>q2 = {static_cast<Type>(1),p2.y};
//                   PointT<Type>q3 = {p3.x/p2.x , p3.y};
//                   parabola = get_parabola_equation(q1, q2, q3);
//                   pol1[0] = parabola.k;
//                   pol1[1] = parabola.b;
//                   pol1[2] = parabola.d + c;
//                   pol2[0] = parabola.k;
//                   pol2[1] = parabola.b;
//                   pol2[2] = parabola.d;
//
//
//               cout<< "pol2= "<< pol2[0] << "x^2 + "<< pol2[1] <<"x + "<< pol2[2] << " + y = 0 ;"<< endl;
//
//               GetIntervalall<Type, double>(pol1, pol2, I1, I2, I3);
//
//               if(I1.size() > 0) {
//                   A1 = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
//               }
//               if(I2.size() > 0) {
//                   A2 = easy_integral_A2(m, n, s, a, c, pol2, I2);
//               }
//               if(I3.size() > 0) {
//                   A3 = easy_integral_A3(m, n, s, a, c, pol2, I3);
//               }
//
//               cout << " A1 = " << A1 << " A2 = " << A2 << " A3 = " << A3 <<endl;
//               area = A1 + A2 + A3;
//
//               cout <<" previous area = "<< area<<endl;
//                   area = area/pow(p2.x , m+1);
//               cout <<" scaling "<< pow(p2.x,m+1) <<" area = "<< area<<endl;
//               }
//             }
//         }
//       }

//     int count =0;
//     srand(10);
//     for (int i = 0; i < 1000; ++i) {
//         double randomX = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
//         double randomY = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
//         double randomZ = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
//         std::vector<std::vector<double>> interp_table;
//         std:: vector <double> interp_point = {randomX,randomY,randomZ};
//         Point3D searchPoint(randomX, randomY, randomZ);
//         OctreeNode<Type>* result = roots[table].search(searchPoint);
//         for (size_t i = 0; i < result->corners.size(); ++i) {
//             interp_table.push_back(result->corners[i]);
//         }
//         double inter_area = trilinier_interpolation<double>(interp_table , interp_point) ;
//
//         PointT <Type> p1,p2,p3;
//         get_p1_p2_p3<Type>(table, interp_point, p1, p2, p3);
// //         Type formula_area = find_area(m,n,s,a,c,table, p1,p2, p3);
//         Type formula_area = find_area_2intersection_formula<Type>(m,n,s,a,c,table, p1,p2, p3);
//
//         double relative_difference = fabs(static_cast<double>(formula_area)- inter_area)/static_cast<double>(formula_area);
//         // Store the values in a vector
//
//         if(relative_difference > 0.001) count++;
//
//         pointsVector.push_back({randomX, randomY, randomZ, static_cast<double>(formula_area), inter_area, relative_difference});
//     }
//
//     for (const auto& point : pointsVector) {
//         std::cout << point[0] << ", " << point[1] << ", " << point[2] << ", " << point[3] << ", " << point[4] << ", "<<point[5]<<"\n";
//     }
//     cout<<"number of entry has relative error bigger than 0.1% = " << count <<endl;
//     return 0;


/*


    int itter = 6;
    std::vector<Type> errors_interpolation(0) ;
    std::vector<Type> errors_formula(0) ;
    std::vector<std::vector<double>> errors_by_table{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} ;
    for (int jj =0; jj < itter; jj++ ){
        double gridArea=0;
        Type area(0);
        unsigned int table_number;
        int normal;
        int totalSwapped(0);
        std:: vector <Type> interp_point(0);

        unsigned ccount = 0;
        Type formulaArea(0);

        Circle<Type> circle;
        circle.center = {static_cast<Type>(0.5), static_cast<Type>(0.5)}; // Assuming the circle is centered at (0.5, 0.5)
        circle.radius = static_cast<Type>(0.379); // Assuming the radius of the circle is 0.4

        int grid_n= 5 * pow(2,jj);
//         int grid_n= 10;
        Type grid_size = static_cast<Type>(1) / grid_n;
        for (int i = 0; i < grid_n; ++i) {
            for (int j = 0; j < grid_n; ++j) {
                std::vector<std::vector<double>> interp_table;
                gridArea = 0 ;
                bool isswap = 0;
                // Define the corners of the current grid
                Point<Type> grid_leftdown = {i * grid_size, j * grid_size};
                Point<Type> grid_righttop = {(i + 1) * grid_size, (j + 1) * grid_size};
                std::vector<int> ltrbNumber{0,0,0,0}; //this vector tells us which axis it intersect and how many times. (left,top,right,bottom)

                //all it does find whether a grid top half or bottom half of the circle

                std::vector<Point<Type>> grid_intersections = findGridIntersectionPoints(circle, grid_leftdown, grid_righttop, ltrbNumber , table_number);

                std::vector<Point<Type>> mapped_intersections = mapIntersectionPoints(grid_intersections, grid_leftdown, grid_righttop, table_number, interp_point,isswap);



                if(grid_intersections.size() == 3){
                if(isswap){ // 1st and 2nd quadrant change the normal
                    normal= ((grid_leftdown.x+grid_righttop.x)*0.5 > circle.center.x )? 1 : 0 ;
                    cout << "we used isswap"<<endl;
                    totalSwapped++;
                }
                else  normal= ((grid_leftdown.y+grid_righttop.y)*0.5 > circle.center.y )? 1 : 0 ;

                    Point3D searchPoint(static_cast<double>(interp_point[0]), static_cast<double>(interp_point[1]), static_cast<double>(interp_point[2]));
                    OctreeNode<Type>* result = roots[table_number].search(searchPoint);
                    // Display the result
                    cout<< "tables = "<<table_number << endl;


                    for (size_t i = 0; i < result->corners.size(); ++i) {

                        interp_table.push_back(result->corners[i]);
                    }

                    std::cout << "Interp Table:\n";
                    for (const auto& corner : interp_table) {
                        std::cout << "(" << corner[0] << ", " << corner[1] << ", " << corner[2] << ", "<< corner[3] <<")\n";
                    }

                //TODO we are going to use that normal at the end ; already have interpolation table
                }

    //               This gridArea is the area in referance space (in mapped space) then scaled down.

    //  double findGridArea(const Circle<Type> &circle, const Point<Type> &gridStart, const Point<Type> &gridEnd, std::vector<Point<Type>> grid_intersections, std:: vector <std:: vector <double>> interp_table, const std::vector< double > &interp_point, const Type &grid_size)


                gridArea = findGridArea<Type>(circle, grid_leftdown, grid_righttop, grid_intersections,interp_table, interp_point,grid_size);

                if(grid_intersections.size() == 3){
                        gridArea = (normal == 0)? gridArea : (static_cast<double>(grid_size)*static_cast<double>(grid_size)  - gridArea) ;
                        cout << "normal =" << normal << " normal_grid area =     "<< gridArea ;
                }

                // direct integral
                PointT <Type> p1,p2,p3;
                Parabola <Type> parabola ;
                Type c(1) , area_b4(0) , area_aftr(0);
                std::vector< std::pair <Type, Type> > I1, I2, I3 ;


                {
                if (mapped_intersections.size() >= 3){ // find the parabola equation from first three points.
                    ccount ++;

                    p1 = { static_cast<Type>(mapped_intersections[0].x), static_cast<Type>(mapped_intersections[0].y) };
                    p2 = { static_cast<Type>(mapped_intersections[1].x), static_cast<Type>(mapped_intersections[1].y) };
                    p3 = { static_cast<Type>(mapped_intersections[2].x), static_cast<Type>(mapped_intersections[2].y) };

                    Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x) ;

                    parabola = get_parabola_equation(p1, p2, p3);

                    Type k, b, d, a(0);

                    std::vector <Type> pol1(3, 0);
                    std::vector <Type> pol2(3, 0);
                    Type A1 = 0, A2 = 0, A3 = 0;

                    if(normal == 0){
                    c=static_cast<Type>(1);
                    pol1[0] = parabola.k ; pol1[1] = a + parabola.b; pol1[2] = c + parabola.d;  pol2[0] = parabola.k; pol2[1] = parabola.b; pol2[2] = parabola.d;
                    }
                    else if(normal ==1){
                    c=static_cast<Type>(-1);
                    pol1[0] = -parabola.k ; pol1[1] = -a - parabola.b; pol1[2] = c - parabola.d;  pol2[0] = -parabola.k; pol2[1] = -parabola.b; pol2[2] = -parabola.d;
                    }

    //                 cout << " prabola equation from 3 points = " <<pol2[0]<<"x^2 + " << pol2[1]<<" x + "<< pol2[2] << " + " << c << "y "<< endl;



                    GetIntervalall<Type, Type>(pol1, pol2, I1, I2, I3);

                    if(I1.size() > 0) {
                    A1 = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
                    }
                    if(I2.size() > 0) {
                    A2 = easy_integral_A2(m, n, s, a, c, pol2, I2);
                    }
                    if(I3.size() > 0) {
                    A3 = easy_integral_A3(m, n, s, a, c, pol2, I3);
                    }

                    area_b4 = (A1 + A2 + A3);

                    area_aftr = (A1 + A2 + A3)*grid_size*grid_size ;

                    formulaArea += area_aftr;
                    if(table_number<8) errors_by_table[table_number][0] +=1;
                    errors_by_table[table_number][1] += static_cast<double>(area_b4 - gridArea/grid_size/grid_size);
                }
                else formulaArea += gridArea;
                }






                // Output the intersection points of the circle with the current grid



    //             for (const Point& intersection : intersections) {
    //                 std::cout << "(" << intersection.x << ", " << intersection.y << ") ";
    //             }



                area += gridArea;

                if(gridArea>0 && gridArea/grid_size/grid_size < 1 && (area_b4 - gridArea/grid_size/grid_size) > 0.0000000001){
//                 if( fabs(gridArea - area_aftr)/area_aftr > 0.01) {
                cout << "\n =========================================================================================" <<endl;
                cout << "Grid = (" << i << ", " << j << "): ";
                cout << " location =(" << i*grid_size << ", " << j*grid_size << "): ";
                cout <<" table number aa :" << table_number << " grid size =" << grid_size <<endl;
                for (int ap = 0; ap < grid_intersections.size();ap++){
                    std::cout << " (" << grid_intersections[ap].x << ", " << grid_intersections[ap].y << ") "<< endl;
                    std::cout << "(" << mapped_intersections[ap].x << "__ " << mapped_intersections[ap].y << ") ";
                }

                for (int ap = 0; ap < mapped_intersections.size();ap++){

                    std::cout << ".(" << mapped_intersections[ap].x << "__ " << mapped_intersections[ap].y << ") ";
                }

                cout<< " interpolated grid area BEFORE scaling = " << gridArea/grid_size/grid_size <<endl;
                cout<< " formula grid area before scaling = " << area_b4 <<endl;
                cout<< " difference before scaling = " << area_b4 - gridArea/grid_size/grid_size <<endl;
                cout << " relative error  : " << (gridArea - area_aftr)/gridArea <<endl;
                cout<< " interpolated grid area after scaling = " << gridArea <<endl;
                cout<< " formula grid area after scaling = " << area_aftr <<endl;
                for(unsigned i = 0; i < I1.size(); i++) {
                    std::cout << "I1_1 = " << I1[i].first << "; I1_2 = " << I1[i].second << ";" << std::endl;
                }
                for(unsigned i = 0; i < I2.size(); i++) {
                    std::cout << "I2_1 = " << I2[i].first << "; I2_2 = " << I2[i].second << ";" << std::endl;
                }
                for(unsigned i = 0; i < I3.size(); i++) {
                    std::cout << "I3_1 = " << I3[i].first << "; I3_2 = " << I3[i].second << ";" << std::endl;
                }

                cout<< " formula area total = " << formulaArea <<endl;
                cout << " difference between direct and interpolation : " << (gridArea - area_aftr) <<endl;

                cout << "LTRB " << ltrbNumber[0] <<ltrbNumber[1]<< ltrbNumber[2]<< ltrbNumber[3]<< endl;
                cout << " 3 points :  " << "(" << p1.x <<" , "<< p1.y<<")" << "(" << p2.x <<" , "<< p2.y<<")"<< "(" << p3.x <<" , "<< p3.y<<")" << endl;
// // // //                 cout << " prabola equation from 3 points = " <<parabola.k<<"x^2 + " << parabola.b<<" x + "<< parabola.d << " + " << /*c <<*/ /*"y = 0 "<< endl;*/
//                 std::cout << " Total area = " << area<<endl;
//                 cout<< " ###############################################################################  "<<endl;
//                 cout<<endl;
//                 }
//             }
//         }
//         cout<< " error by table = " << endl;
//         for(int er=0; er < 8 ; er++){
//          cout<< " table = " << er << " "<< errors_by_table[er][0]  << " "<< errors_by_table[er][1] <<endl;
//
//         }
//
//         std::cout << "Total area = " << area  << "formulaArea = " <<formulaArea << " original area = " << circle.radius * circle.radius * M_PI << " interpolation error = " << fabs(area - circle.radius * circle.radius * M_PI ) << " formula error = " <<fabs(formulaArea - circle.radius * circle.radius * M_PI ) << endl;
//
//         errors_interpolation.push_back(fabs(area - circle.radius * circle.radius * M_PI ));
//         errors_formula.push_back(fabs(formulaArea - circle.radius * circle.radius * M_PI ));
//     }
//
//             cout << "\n interpolation error = ";
//             for (int jj =0; jj<itter; jj++ ){
//                 cout << errors_interpolation[jj] << " " ;
//             }
//
//             cout << "\n formula error = ";
//             for (int jj =0; jj<itter; jj++ ){
//                 cout << errors_formula[jj] << " " ;
//             }
//
//             cout << "\n interpolation convergence = ";
//             for (int jj =1; jj<itter; jj++ ){
//                 cout << log2(errors_interpolation[jj-1] / errors_interpolation[jj]) << " " ;
//             }
//
//             cout << "\n formula convergence = ";
//             for (int jj =1; jj<itter; jj++ ){
//                 cout << log2(errors_formula[jj-1] / errors_formula[jj]) << " " ;
//             }
//
//         return 0;
//
// }




 //Number of grids
    int itter = 7;
    std::vector<Type> errors_interpolation(0) ;
    std::vector<Type> errors_formula(0) ;
    std::vector<std::vector<double>> errors_by_table{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} ;
    for (int jj =0; jj< itter; jj++ ){
        Type gridArea=0;
        Type area(0);
        unsigned int table_number;
        int normal;
        int totalSwapped(0);
        std:: vector <Type> interp_point(0);


        unsigned ccount = 0;
        Type formulaArea(0);

        Circle<Type> circle;
        circle.center = {static_cast<Type>(0.5), static_cast<Type>(0.5)}; // Assuming the circle is centered at (0.5, 0.5)
        circle.radius = static_cast<Type>(0.379); // Assuming the radius of the circle is 0.4

        int grid_n= 5 * pow(2,jj);
//         int grid_n= 10;
        Type grid_size = static_cast<Type>(1) / grid_n;
        for (int i = 0; i < grid_n; ++i) {
            for (int j = 0; j < grid_n; ++j) {

                PointT <Type> p1,p2,p3;
                Parabola <Type> parabola ;
                Type c(1) , area_b4(0) , area_aftr(0);
                std::vector< std::pair <Type, Type> > I1, I2, I3 ;
                std::vector<std::vector<double>> interp_table;
                gridArea = 0 ;
                bool isswap = 0;
//                 Define the corners of the current grid
                Point<Type> grid_leftdown = {i * grid_size, j * grid_size};
                Point<Type> grid_righttop = {(i + 1) * grid_size, (j + 1) * grid_size};
                std::vector<int> ltrbNumber{0,0,0,0}; //this vector tells us which axis it intersect and how many times. (left,top,right,bottom)

//                 all it does find whether a grid top half or bottom half of the circle

                std::vector<Point<Type>> grid_intersections = findGridIntersectionPoints(circle, grid_leftdown, grid_righttop, ltrbNumber , table_number);

                std::vector<Point<Type>> mapped_intersections = mapIntersectionPoints(grid_intersections, grid_leftdown, grid_righttop, table_number, interp_point,isswap);

                if(grid_intersections.size() <= 1){
//                   checking wherher the grid is outside or inside the circle . Circle equation: (x - h)^2 + (y - k)^2 = r^2 Find (i+epsilon-h)^2 +(j+epsilon-k)^2 < r^2
                  gridArea = ((grid_leftdown.x - circle.center.x )*(grid_leftdown.x - circle.center.x) + (grid_leftdown.y - circle.center.y)*(grid_leftdown.y - circle.center.y) < (circle.radius*circle.radius)) ? 1 : 0 ;
                }

                if(grid_intersections.size() == 3){
                if(isswap){ // 1st and 2nd quadrant change the normal
                    normal= ((grid_leftdown.x+grid_righttop.x)*0.5 > circle.center.x )? 1 : 0 ;
                    cout << "we used isswap"<<endl;
                    totalSwapped++;
                }
                else  normal= ((grid_leftdown.y+grid_righttop.y)*0.5 > circle.center.y )? 1 : 0 ;

                    Point3D searchPoint(static_cast<double>(interp_point[0]), static_cast<double>(interp_point[1]), static_cast<double>(interp_point[2]));
                    OctreeNode<Type>* result = roots[table_number].search(searchPoint);
//                     Display the result
                    cout<< "tables = "<<table_number << endl;


                    for (size_t i = 0; i < result->corners.size(); ++i) {

                        interp_table.push_back(result->corners[i]);
                    }

                    std::cout << "Interp Table:\n";
                    for (const auto& corner : interp_table) {
                        std::cout << "(" << corner[0] << ", " << corner[1] << ", " << corner[2] << ", "<< corner[3] <<")\n";
                    }
//                 TODO we are going to use that normal at the end ; already have interpolation table

                p1 = { static_cast<Type>(mapped_intersections[0].x), static_cast<Type>(mapped_intersections[0].y) };
                p2 = { static_cast<Type>(mapped_intersections[1].x), static_cast<Type>(mapped_intersections[1].y) };
                p3 = { static_cast<Type>(mapped_intersections[2].x), static_cast<Type>(mapped_intersections[2].y) };

                parabola = get_parabola_equation(p1, p2, p3);
                parabola = {-parabola.k,-parabola.b,-parabola.d};

                  if (table_number == 2){
                    gridArea = (1-parabola.d - parabola.k*p2.x*p2.x/3 - parabola.b*p2.x/2)*p2.x - (1-parabola.d - parabola.k*p1.x*p1.x/3 - parabola.b*p1.x/2)*p1.x + 1 -p2.x ;
                  }
                  else if (table_number == 6){
                    gridArea = (1-parabola.d - parabola.k*p1.x*p1.x/3 - parabola.b*p1.x/2)*p1.x - (1-parabola.d - parabola.k*p2.x*p2.x/3 - parabola.b*p2.x/2)*p2.x + p2.x ;
                  }
                  else{
                    gridArea = (1-parabola.d - parabola.k*p2.x*p2.x/3 - parabola.b*p2.x/2)*p2.x - (1-parabola.d - parabola.k*p1.x*p1.x/3 - parabola.b*p1.x/2)*p1.x;
                  }
                }

                cout << " grid size = " << grid_size *grid_size ;
                cout << " grid area = " << gridArea <<endl;
                gridArea = gridArea * static_cast<double>(grid_size) * static_cast<double>(grid_size) ;


                if(grid_intersections.size() == 3){
                        gridArea = (normal == 0)? gridArea : (static_cast<double>(grid_size)*static_cast<double>(grid_size)  - gridArea) ;
                        cout << "normal =" << normal << " normal_grid area =     "<< gridArea ;
                }

//                 direct integral



                {
                if (mapped_intersections.size() >= 3){ // find the parabola equation from first three points.
                    ccount ++;

                    p1 = { static_cast<Type>(mapped_intersections[0].x), static_cast<Type>(mapped_intersections[0].y) };
                    p2 = { static_cast<Type>(mapped_intersections[1].x), static_cast<Type>(mapped_intersections[1].y) };
                    p3 = { static_cast<Type>(mapped_intersections[2].x), static_cast<Type>(mapped_intersections[2].y) };

                    Type det = p1.x * p1.x * (p2.x - p3.x) -p1.x* (p2.x*p2.x - p3.x*p3.x)+ p2.x*p3.x*(p2.x - p3.x) ;

                    parabola = get_parabola_equation(p1, p2, p3);

                    Type k, b, d, a(0);

                    std::vector <Type> pol1(3, 0);
                    std::vector <Type> pol2(3, 0);
                    Type A1 = 0, A2 = 0, A3 = 0;

                    if(normal == 0){
                    c=static_cast<Type>(1);
                    pol1[0] = parabola.k ; pol1[1] = a + parabola.b; pol1[2] = c + parabola.d;  pol2[0] = parabola.k; pol2[1] = parabola.b; pol2[2] = parabola.d;
                    }
                    else if(normal ==1){
                    c=static_cast<Type>(-1);
                    pol1[0] = -parabola.k ; pol1[1] = -a - parabola.b; pol1[2] = c - parabola.d;  pol2[0] = -parabola.k; pol2[1] = -parabola.b; pol2[2] = -parabola.d;
                    }

                    cout << " prabola equation from 3 points = " <<pol2[0]<<"x^2 + " << pol2[1]<<" x + "<< pol2[2] << " + " << c << "y "<< endl;



                    GetIntervalall<Type, Type>(pol1, pol2, I1, I2, I3);

                    if(I1.size() > 0) {
                    A1 = easy_integral_A3(m, n, s, a, c, pol2, I1) -  easy_integral_A2(m, n, s, a, c, pol2, I1);
                    }
                    if(I2.size() > 0) {
                    A2 = easy_integral_A2(m, n, s, a, c, pol2, I2);
                    }
                    if(I3.size() > 0) {
                    A3 = easy_integral_A3(m, n, s, a, c, pol2, I3);
                    }

                    area_b4 = (A1 + A2 + A3);

                    area_aftr = (A1 + A2 + A3)*grid_size*grid_size ;

                    formulaArea += area_aftr;
                    if(table_number<8) errors_by_table[table_number][0] +=1;
                    errors_by_table[table_number][1] += static_cast<double>(area_b4 - gridArea/grid_size/grid_size);
                }
                else formulaArea += gridArea;
                }






//                 Output the intersection points of the circle with the current grid



//                 for (const Point& intersection : intersections) {
//                     std::cout << "(" << intersection.x << ", " << intersection.y << ") ";
//                 }



                area += gridArea;

//                 if(gridArea!=0 && gridArea/grid_size/grid_size != 1){
//                 cout << "------------------------" <<endl;
//                 cout << "Grid = (" << i << ", " << j << "): ";
//                 cout << " location =(" << i*grid_size << ", " << j*grid_size << "): ";
//                 cout <<" table number aa :" << table_number << " grid size =" << grid_size <<endl;
//                 for (int ap = 0; ap < grid_intersections.size();ap++){
//                     std::cout << " (" << grid_intersections[ap].x << ", " << grid_intersections[ap].y << ") "<< endl;
//                     std::cout << "(" << mapped_intersections[ap].x << "__ " << mapped_intersections[ap].y << ") ";
//                 }
//
//                 for (int ap = 0; ap < mapped_intersections.size();ap++){
//
//                     std::cout << ".(" << mapped_intersections[ap].x << "__ " << mapped_intersections[ap].y << ") ";
//                 }
//
//                 cout<< " interpolated grid area BEFORE scaling = " << gridArea/grid_size/grid_size <<endl;
//                 cout<< " formula grid area before scaling = " << area_b4 <<endl;
//                 cout<< " difference before scaling = " << area_b4 - gridArea/grid_size/grid_size <<endl;
//                 cout<< " interpolated grid area after scaling = " << gridArea <<endl;
//                 cout<< " formula grid area after scaling = " << area_aftr <<endl;
//                 for(unsigned i = 0; i < I1.size(); i++) {
//                     std::cout << "I1_1 = " << I1[i].first << "; I1_2 = " << I1[i].second << ";" << std::endl;
//                 }
//                 for(unsigned i = 0; i < I2.size(); i++) {
//                     std::cout << "I2_1 = " << I2[i].first << "; I2_2 = " << I2[i].second << ";" << std::endl;
//                 }
//                 for(unsigned i = 0; i < I3.size(); i++) {
//                     std::cout << "I3_1 = " << I3[i].first << "; I3_2 = " << I3[i].second << ";" << std::endl;
//                 }
//
//                 cout<< " formula area total = " << formulaArea <<endl;
//                 cout << " difference between direct and interpolation : " << (gridArea - area_aftr) <<endl;
//                 cout << "LTRB " << ltrbNumber[0] <<ltrbNumber[1]<< ltrbNumber[2]<< ltrbNumber[3]<< endl;
//                 cout << " 3 points :  " << "(" << p1.x <<" , "<< p1.y<<")" << "(" << p2.x <<" , "<< p2.y<<")"<< "(" << p3.x <<" , "<< p3.y<<")" << endl;
//                 cout << " prabola equation from 3 points = " <<parabola.k<<"x^2 + " << parabola.b<<" x + "<< parabola.d << " + " << /*c <<*/ "y "<< endl;
//                 std::cout << " Total area = " << area<<endl;
//                 cout<< " ###############################################################################  "<<endl;
//                 cout<<endl;
//                 }
            }
        }
        cout<< " error by table = " << endl;
        for(int er=0; er < 8 ; er++){
         cout<< " table = " << er << " "<< errors_by_table[er][0]  << " "<< errors_by_table[er][1] <<endl;

        }

        std::cout << "Total area = " << area  << "formulaArea = " <<formulaArea << " original area = " << circle.radius * circle.radius * M_PI << " interpolation error = " << fabs(area - circle.radius * circle.radius * M_PI ) << " formula error = " <<fabs(formulaArea - circle.radius * circle.radius * M_PI ) << endl;

        errors_interpolation.push_back(fabs(area - circle.radius * circle.radius * M_PI ));
        errors_formula.push_back(fabs(formulaArea - circle.radius * circle.radius * M_PI ));
    }

            cout << "\n interpolation error = ";
            for (int jj =0; jj<itter; jj++ ){
                cout << errors_interpolation[jj] << " " ;
            }

            cout << "\n formula error = ";
            for (int jj =0; jj<itter; jj++ ){
                cout << errors_formula[jj] << " " ;
            }

            cout << "\n interpolation convergence = ";
            for (int jj =1; jj<itter; jj++ ){
                cout << log2(errors_interpolation[jj-1] / errors_interpolation[jj]) << " " ;
            }

            cout << "\n formula convergence = ";
            for (int jj =1; jj<itter; jj++ ){
                cout << log2(errors_formula[jj-1] / errors_formula[jj]) << " " ;
            }

//         printOctreeStructure(&root);

    return 0;

}








