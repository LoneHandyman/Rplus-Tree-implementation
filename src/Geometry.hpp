#pragma once

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

    double min_dist(const std::vector<double>& point);
  };

}

#include "../source/Geometry.cpp"
