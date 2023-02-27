#pragma once

#include "RplusTree.hpp"
#include "Geometry.hpp"
#include "CsvWrapper.hpp"
#include <tuple>
#include <iostream>
#include <chrono>

const unsigned int dims = 13;

typedef typename std::tuple<double, double, int, double, int, double, double, double, double, double, double, int, int, std::string> SpotifyData;

void print_spotify_data(SpotifyData& single_data){
  std::cout << '[';
  std::cout << std::get<0>(single_data) << ", ";
  std::cout << std::get<1>(single_data) << ", ";
  std::cout << std::get<2>(single_data) << ", ";
  std::cout << std::get<3>(single_data) << ", ";
  std::cout << std::get<4>(single_data) << ", ";
  std::cout << std::get<5>(single_data) << ", ";
  std::cout << std::get<6>(single_data) << ", ";
  std::cout << std::get<7>(single_data) << ", ";
  std::cout << std::get<8>(single_data) << ", ";
  std::cout << std::get<9>(single_data) << ", ";
  std::cout << std::get<10>(single_data) << ", ";
  std::cout << std::get<11>(single_data) << ", ";
  std::cout << std::get<12>(single_data) << ", ";
  std::cout << std::get<13>(single_data);
  std::cout << "]\n";
}

kd::Rect<dims> get_boundingbox(SpotifyData& single_data){
  std::vector<double> position;
  position.push_back(static_cast<double>(std::get<0>(single_data)));
  position.push_back(static_cast<double>(std::get<1>(single_data)));
  position.push_back(static_cast<double>(std::get<2>(single_data)));
  position.push_back(static_cast<double>(std::get<3>(single_data)));
  position.push_back(static_cast<double>(std::get<4>(single_data)));
  position.push_back(static_cast<double>(std::get<5>(single_data)));
  position.push_back(static_cast<double>(std::get<6>(single_data)));
  position.push_back(static_cast<double>(std::get<7>(single_data)));
  position.push_back(static_cast<double>(std::get<8>(single_data)));
  position.push_back(static_cast<double>(std::get<9>(single_data)));
  position.push_back(static_cast<double>(std::get<10>(single_data)));
  position.push_back(static_cast<double>(std::get<11>(single_data)));
  position.push_back(static_cast<double>(std::get<12>(single_data)));
  kd::Rect<dims> bounding_box(position, position);
  return bounding_box;
}

void read_dataset(std::vector<SpotifyData>& data){
  io::CSVReader<14> in("csv/genres_v2.csv");
  in.read_header(io::ignore_extra_column, "danceability", "energy", "key", "loudness", "mode", "speechiness", "acousticness", "instrumentalness", "liveness", "valence", "tempo", "duration_ms", "time_signature", "song_name");
  double danceability, energy, loudness, speechiness, acousticness, instrumentalness, liveness, valence, tempo; 
  int key, duration_ms, time_signature, mode;
  std::string song_name;
  int count = 500;
  SpotifyData single_data;

  while(count-- && in.read_row(danceability, energy, key, loudness, mode, speechiness, acousticness, instrumentalness, liveness, valence, tempo, duration_ms, time_signature, song_name)){
    single_data = std::make_tuple(danceability, energy, key, loudness, mode, speechiness, acousticness, instrumentalness, liveness, valence, tempo, duration_ms, time_signature, song_name);
    data.push_back(single_data);
  }
  std::cout << "# Rows: " << data.size() << '\n';
}

void insertion_step(kd::RPlusTree<int, dims, 50, 20>& tree, std::vector<SpotifyData>& data){
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
  start = std::chrono::high_resolution_clock::now();
  for (unsigned int i = 0; i < data.size(); ++i){
    tree.insert(i, get_boundingbox(data[i]));
  }
  end = std::chrono::high_resolution_clock::now();
  unsigned long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  std::cout << "Insertion (elapsed time): " + std::to_string(duration) + " ns.\n";
}

enum ManOptions {
  Ignored, PrintStructure, CountNodes, KnnSearch, Quit
};

void print_legend(){
  std::cout << "R+ Tree manager options:\n";
  std::cout << "~ To print the structure of the tree write: " << ManOptions::PrintStructure << '\n';
  std::cout << "~ To print the number of nodes of the tree write: " << ManOptions::CountNodes << '\n';
  std::cout << "~ To do a knn search in the tree based on a single point write: " << ManOptions::KnnSearch << '\n';
  std::cout << "~ If you want to exit write: " << ManOptions::Quit << '\n';
}

void rplus_test(){
  kd::RPlusTree<int, dims, 50, 20> tree;
  std::vector<SpotifyData> spotify_data_set;
  read_dataset(spotify_data_set);
  insertion_step(tree, spotify_data_set);
  ManOptions option = Ignored;
  while(option != ManOptions::Quit){
    std::cout << "-/------***++++++++++++***------/-\n";
    print_legend();
    unsigned int raw_op;
    std::cin >> raw_op;
    option = static_cast<ManOptions>(raw_op);
    if (option == PrintStructure){
      tree.print_structure();
    }
    else if (option == CountNodes){
      std::cout << ">> Num. of nodes: " << tree.count_nodes() << '\n';
    }
    else if (option == KnnSearch){
      unsigned int k;
      std::vector<double> point(dims);
      std::cout << ">> Write how many neighbors you want to get: ";
      std::cin >> k;
      std::cout << ">> Write the point to start knn: ";
      for (unsigned int i = 0; i < dims; ++i){
        std::cin >> point[i];
      }

      std::vector<int> knn = tree.knn_query(point, k);
      std::cout << ">> Num. of neighbors returned: " << knn.size() << '\n';
      for (unsigned int i = 0; i < knn.size(); ++i){
        std::cout << '\t' << knn[i] << ". ";
        print_spotify_data(spotify_data_set[knn[i]]);
      }
    }
  }
}
