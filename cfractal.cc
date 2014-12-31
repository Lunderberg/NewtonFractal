#include <complex>
#include <iostream>
#include <string>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
namespace bp = boost::python;

typedef std::vector<std::complex<double>> complex_list;

void from_c(){
  std::cout << "Hello from C++" << std::endl;
}

double square(double x){
  return x*x;
}


template<typename T>
std::vector<T> convert_list(const bp::list& pylist){
  std::vector<T> out;
  for(int i=0; i<bp::len(pylist); i++){
    out.push_back(bp::extract<T>(pylist[i]));
  }
  return out;
}

class Polynomial{
public:
  Polynomial(){ }

  Polynomial(const complex_list& coeffs) :
    coeffs_(coeffs) { }

  static Polynomial FromZeros(const complex_list& zeros){
    int n = zeros.size();

    complex_list coeffs;
    coeffs.resize(n+1, 0);

    // The bitpattern indicates which zeros should be multiplied together.
    for(unsigned long bitpat=0; bitpat < (1<<n); bitpat++){
      std::complex<double> cumprod = 1;
      int power = n;
      for(unsigned int i=0; i<n; i++){
        if(bitpat & (1<<i)){
          cumprod *= -zeros[i];
          power--;
        }
      }
      coeffs.at(n-power) += cumprod;
    }


    return Polynomial(coeffs);
  }

  Polynomial Derivative() const {
    complex_list new_coeffs;
    for(unsigned int i=0; i<coeffs_.size()-1; i++){
      double power = coeffs_.size() - i - 1;
      new_coeffs.push_back(double(power)*coeffs_[i]);
    }
    return Polynomial(new_coeffs);
  }

  std::complex<double> operator() (std::complex<double> z) const {
    std::complex<double> out = 0;
    for(auto coeff : coeffs_){
      out *= z;
      out += coeff;
    }
    return out;
  }

  friend std::ostream& operator<<(std::ostream& out, const Polynomial& poly){
    for(unsigned int i=0; i<poly.coeffs_.size(); i++){
      int power = poly.coeffs_.size() - i - 1;
      out << "(" << poly.coeffs_[i].real() << " + " << poly.coeffs_[i].imag() << "i)z";
      if(power>1){
        out << "**" << power << " + ";
      } else if (power==1){
        out << " + ";
      }
    }
    return out;
  }

private:
  complex_list coeffs_;
};

class NewtonFractal{
public:
  NewtonFractal(const complex_list& zeros){
    SetZeros(zeros);
  }
  NewtonFractal(const bp::list& zeros){
    SetZeros(convert_list<std::complex<double>>(zeros));
  }

  void SetZeros(const complex_list& zeros){
    zeros_ = zeros;
    poly_ = Polynomial::FromZeros(zeros);
    polyder_ = poly_.Derivative();
  }

  bp::tuple eval(std::complex<double> guess) const {
    int max_iter = 250;
    double threshold = 1e-5;

    int iterations;
    for(iterations=0; iterations<max_iter; iterations++){
      std::complex<double> polyval = poly_(guess);
      if(std::abs(polyval) < threshold){
        break;
      }
      guess -= polyval/polyder_(guess);
    }

    // No success if no convergence.
    if(iterations==max_iter){
      return bp::make_tuple(-1,max_iter);
    }

    // Find the closest zero.
    int zero_index = -1;
    double min_dist = 1e100;
    for(unsigned int i=0; i<zeros_.size(); i++){
      double dist = std::abs(zeros_[i] - guess);
      if(dist < min_dist){
        zero_index = i;
        min_dist = dist;
      }
    }

    return bp::make_tuple(zero_index, iterations);
  }


private:
  complex_list zeros_;
  Polynomial poly_;
  Polynomial polyder_;
};

BOOST_PYTHON_MODULE(cfractal){
  bp::def("from_c", from_c);
  bp::def("square", square);
  bp::class_<NewtonFractal>("NewtonFractal", bp::init<const bp::list&>())
    .def("eval", &NewtonFractal::eval)
    ;
}
