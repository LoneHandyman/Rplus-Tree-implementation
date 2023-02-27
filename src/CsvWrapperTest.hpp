#pragma once

#include "CsvWrapper.hpp"
#include <iostream>

void csv_test(){
  io::CSVReader<3> in("csv/genres_v2.csv");
  in.read_header(io::ignore_extra_column, "song_name", "energy", "key");
  std::string danceability; double energy; int key;
  int i = 1;

  while(in.read_row(danceability, energy, key) && i != 10){
    std::cout << i << ':'<< danceability << ' ' << energy << ' ' << key << '\n';
    ++i;
  }
}