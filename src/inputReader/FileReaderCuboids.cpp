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
  std::ifstream input_file(filename);
  std::string tmp_string;

  if (!input_file.is_open()) {
    std::cout << "Error: could not open file " << filename << std::endl;
    exit(-1);
  }

  getline(input_file, tmp_string);
  std::cout << "Read line: " << tmp_string << std::endl;

  while (tmp_string.empty() || tmp_string[0] == '#') {
    if (!getline(input_file, tmp_string)) {
      std::cout << "Error: reached end of file before reading cuboid count.\n";
      exit(-1);
    }
    std::cout << "Read line: " << tmp_string << std::endl;
  }

  int num_particles = 0;
  std::istringstream numstream(tmp_string);
  numstream >> num_particles;
  std::cout << "Reading " << num_particles << "." << std::endl;

  for (int i = 0; i < num_particles; i++) {
    if (!getline(input_file, tmp_string)) {
      std::cout << "Error reading file: expected " << num_particles << " cuboids but only found " << i << "."
                << std::endl;
      exit(-1);
    }
    std::cout << "Read line: " << tmp_string << std::endl;

    std::istringstream datastream(tmp_string);
    Cuboid cuboid{};

    for (auto &coord : cuboid.origin) {
      datastream >> coord;
    }
    for (auto &count : cuboid.numPerDim) {
      datastream >> count;
    }
    for (auto &vel : cuboid.baseVelocity) {
      datastream >> vel;
    }
    if (datastream.eof()) {
      std::cout << "Error reading file: eof reached unexpectedly reading from line " << i << std::endl;
      exit(-1);
    }
    datastream >> cuboid.h;
    datastream >> cuboid.mass;

    cuboids.push_back(cuboid);
  }
}
