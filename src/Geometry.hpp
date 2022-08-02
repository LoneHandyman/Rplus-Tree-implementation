#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include <iostream>
#include <algorithm>
#include <vector>

namespace kd{

  template<uint32_t dim>
  class Rect{
  private:
    double low[dim], high[dim];
  public:
    Rect();
    Rect(const std::vector<double>& low_point, const std::vector<double>& high_point);
    Rect(double low_point[], double high_point[]);
    Rect(const Rect& other);

    Rect& operator=(const Rect& other);
    void expand(const Rect& other);

    bool overlaps(const Rect& other);
    double get_area() const;

    Rect cut(const double& cutline, const uint32_t& axis);

    double get_low_axis(const uint32_t& axis) const;
    double get_high_axis(const uint32_t& axis) const;
  };

}

template<uint32_t dim>
kd::Rect<dim>::Rect(){
  for (uint32_t i = 0; i < dim; ++i){
    low[i] = high[i] = 0.0;
  }
}

template<uint32_t dim>
kd::Rect<dim>::Rect(const std::vector<double>& low_point, const std::vector<double>& high_point){
  for (uint32_t i = 0; i < dim; ++i){
    low[i] = low_point[i];
    high[i] = high_point[i];
  }
}

template<uint32_t dim>
kd::Rect<dim>::Rect(double low_point[], double high_point[]){
  for (uint32_t i = 0; i < dim; ++i){
    low[i] = low_point[i];
    high[i] = high_point[i];
  }
}

template<uint32_t dim>
kd::Rect<dim>::Rect(const Rect& other){
  *this = other;
}

template<uint32_t dim>
kd::Rect<dim>& kd::Rect<dim>::operator=(const Rect& other){
  if (this != &other){
    for (uint32_t i = 0; i < dim; ++i){
      low[i] = other.low[i];
      high[i] = other.high[i];
    }
  }
  return *this;
}

template<uint32_t dim>
void kd::Rect<dim>::expand(const Rect& other){
  for (uint32_t i = 0; i < dim; ++i){
    low[i] = std::min(low[i], other.low[i]);
    high[i] = std::max(high[i], other.high[i]);
  }
}

template<uint32_t dim>
bool kd::Rect<dim>::overlaps(const Rect& other){
  for (uint32_t i = 0; i < dim; ++i){
    if((low[i] > other.high[i]) || (high[i] < other.low[i])){
      return false;
    }
  }
  return true;
}

template<uint32_t dim>
double kd::Rect<dim>::get_area() const{
  double area = 1.0;
  for (uint32_t i = 0; i < dim; ++i){
    area *= (high[i] - low[i]);
  }
  return area;
}

template<uint32_t dim>
kd::Rect<dim> kd::Rect<dim>::cut(const double& cutline, const uint32_t& axis){
  Rect right_side = *this;
  high[axis] = cutline;
  right_side.low[axis] = cutline;
  return right_side;
}

template<uint32_t dim>
double kd::Rect<dim>::get_low_axis(const uint32_t& axis) const{
  return low[axis];
}

template<uint32_t dim>
double kd::Rect<dim>::get_high_axis(const uint32_t& axis) const{
  return high[axis];
}
#endif