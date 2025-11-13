//
// Created by darig on 11/13/2025.
//

#include "FileReaderCuboids.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

FileReaderCuboid::FileReaderCuboid() = default;

FileReaderCuboid::~FileReaderCuboid() = default;

void FileReaderCuboid::readFile(std::vector<Cuboid>& cuboids, char *filename) {
  std::array<double, 3> origin{};
  std::array<size_t, 3> numPerDim{};
  std::array<double, 3> baseVelocity{};
  double h;
  double mass;
  int num_particles = 0;


  std::ifstream input_file(filename);
  std::string tmp_string;

  if (input_file.is_open()) {
    getline(input_file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

    while (tmp_string.empty() or tmp_string[0] == '#') {
      getline(input_file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    std::cout << "Reading " << num_particles << "." << std::endl;
    getline(input_file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

    for (int i = 0; i < num_particles; i++) {
      std::istringstream datastream(tmp_string);

      for (auto &oj : origin) {
        datastream >> oj;
      }
      for (auto &nj : numPerDim) {
        datastream >> nj;
      }
      for (auto &vj : baseVelocity) {
        datastream >> vj;
      }
      if (datastream.eof()) {
        std::cout << "Error reading file: eof reached unexpectedly reading from line " << i << std::endl;
        exit(-1);
      }
      datastream >> h;
      datastream >> mass;

      Cuboid cuboid{origin, numPerDim, h, mass, baseVelocity, 0.1, i};
      cuboids.push_back(cuboid);
      getline(input_file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }
  } else {
    std::cout << "Error: could not open file " << filename << std::endl;
    exit(-1);
  }
}