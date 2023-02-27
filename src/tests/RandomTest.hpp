#pragma once

#include "RplusTree.hpp"
#include "Geometry.hpp"
#include <csv.h>
#include <time.h>

kd::Rect<6> get_random_rect(){
  std::vector<double> low, high;
  low.resize(6);
  high.resize(6);
  for (uint32_t i = 0; i < 6; ++i){
    low[i] = double(rand() % 1000) / 2;
    high[i] = low[i];// + (rand() % 4 + 1);
  }
  std::cout << "\tlow: ";
  for (uint32_t i = 0; i < 6; ++i){
    std::cout << low[i] << ' ';
  }
  std::cout << "\n\thigh: ";
  for (uint32_t i = 0; i < 6; ++i){
    std::cout << high[i] << ' ';
  }
  std::cout << '\n';
  kd::Rect<6> rect(low, high);
  return rect;
}

void random_test(){
  srand(time(0));
  kd::RPlusTree<char, 6, 5, 2> rtree;
  std::cout << 'A' << '\n';
  rtree.insert('A', get_random_rect());
  std::cout << 'B' << '\n';
  rtree.insert('B', get_random_rect());
  std::cout << 'C' << '\n';
  rtree.insert('C', get_random_rect());
  std::cout << 'D' << '\n';
  rtree.insert('D', get_random_rect());
  std::cout << 'E' << '\n';
  rtree.insert('E', get_random_rect());
  std::cout << 'F' << '\n';
  rtree.insert('F', get_random_rect());
  std::cout << 'G' << '\n';
  rtree.insert('G', get_random_rect());
  std::cout << 'H' << '\n';
  rtree.insert('H', get_random_rect());
  std::cout << 'I' << '\n';
  rtree.insert('I', get_random_rect());
  std::cout << 'J' << '\n';
  rtree.insert('J', get_random_rect());
  std::cout << 'K' << '\n';
  rtree.insert('K', get_random_rect());
  std::cout << 'L' << '\n';
  rtree.insert('L', get_random_rect());
  std::cout << 'M' << '\n';
  rtree.insert('M', get_random_rect());
  std::cout << 'N' << '\n';
  rtree.insert('N', get_random_rect());
  std::cout << 'O' << '\n';
  rtree.insert('O', get_random_rect());
  std::cout << 'P' << '\n';
  rtree.insert('P', get_random_rect());
  std::cout << 'Q' << '\n';
  rtree.insert('Q', get_random_rect());
  std::cout << 'R' << '\n';
  rtree.insert('R', get_random_rect());
  std::cout << 'S' << '\n';
  rtree.insert('S', get_random_rect());
  std::cout << 'T' << '\n';
  rtree.insert('T', get_random_rect());
  std::cout << 'U' << '\n';
  rtree.insert('U', get_random_rect());
  std::cout << 'V' << '\n';
  rtree.insert('V', get_random_rect());
  std::cout << "# of nodes: " << rtree.count_nodes() << '\n';
  rtree.print_structure();
  std::cout << "Finished\n";
  std::vector<double> point = {284.5, 259, 259.5, 466.5, 78, 249.5};
  std::vector<char> knn = rtree.knn_query(point, 5);
  for (int i = 0; i < knn.size(); ++i){
    std::cout << knn[i] << ' ';
  }
  
  std::cout << '\n';
}
