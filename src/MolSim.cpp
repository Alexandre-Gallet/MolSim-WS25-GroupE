/**
 * @file MolSim.cpp
 * @brief Main entry point for the Molecular Dynamics simulation
 */

// #include <bits/valarray_after.h>
#include <filesystem>
#include <iostream>
#include <list>
#include <valarray>

#include "ForceCalculation/ForceCalculation.h"
#include "ForceCalculation/StormerVerlet.h"
#include "ParticleContainer.h"
#include "PlanetSimulation.h"
#include "SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/FileReader.h"
#include "inputReader/InputReader.h"
#include "outputWriter/OutputFormat.h"
#include "outputWriter/OutputWriter.h"
#include "outputWriter/WriterFactory.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * @brief Calculate the force for all particles
 */
void calculateF();

/**
 * @brief Calculate the position for all particles
 */
void calculateX();

/**
 * @brief Calculate the velocity for all particles
 */
void calculateV();

/**
 * @brief Plot the particles to an output file
 * @param iteration Current iteration number used for output filename
 * @param format The chosen output format
 */
void plotParticles(int iteration, OutputFormat format);

/// Start time of the simulation
double start_time = 0;

/// End time of the simulation
double end_time = 1000;

/// Time step size for integration
double delta_t = 0.014;

/// Container holding all particles in the simulation
ParticleContainer particles;

/**
 * @brief Main entry point of the molecular dynamics simulation
 * @param argc Number of command line arguments
 * @param argsv Array of command line argument strings
 * @return EXIT_SUCCESS (0) on successful completion, -1 on error
 */
int main(int argc, char *argsv[]) {
  Arguments args;
  inputReader::parseArguments(argc, argsv, args);
  auto simulation = SimulationFactory::createSimulation(SimulationType::Planet, args, particles);
  simulation->runSimulation();

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}

void calculateF() {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  for (auto &p1 : particles) {
    for (auto &p2 : particles) {
      // @TODO: insert calculation of forces here!
      if (!(p1 == p2)) {  // calculate the forces according to the given formula in the lecture
        double norm =
            std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 3);
        double scalar = p1.getM() * p2.getM() / norm;
        std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
            scalar, ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()), std::multiplies<>());
        p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
        // p2.setF(ArrayUtils::elementWisePairOp(p2.getF(),newF,  std::minus<>()));
      }
    }
  }
}

void calculateX() {
  // calculate the position updates using the methods in the ArrayUtils class

  for (auto &p : particles) {
    p.setX(ArrayUtils::elementWisePairOp(
        p.getX(),
        ArrayUtils::elementWisePairOp(
            ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
            ArrayUtils::elementWiseScalarOp(pow(delta_t, 2) / (2 * p.getM()), p.getF(), std::multiplies<>()),
            std::plus<>()),
        std::plus<>()));
  }
}

void calculateV() {
  // calculate the forces using the methods in the ArrayUtils class

  for (auto &p : particles) {
    p.setV(ArrayUtils::elementWisePairOp(
        p.getV(),
        ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()),
                                        ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()),
                                        std::multiplies<>()),
        std::plus<>()));
  }
}

/**
 * @brief Write the particles output to a file
 * @param iteration Current iteration number used for output filename
 * @param format The chosen output format
 */
void plotParticles(int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");

  const auto writer = WriterFactory::createWriter(format);

  writer->plotParticles(particles, out_name, iteration);
}