
// #include <bits/valarray_after.h>
#include <filesystem>
#include <iostream>
#include <list>
#include <valarray>

#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#ifdef ENABLE_VTK_OUTPUT
#include "outputWriter/VTKWriter.h"
#endif
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

constexpr double start_time = 0;
double end_time = 1000;
double delta_t = 0.014;

// TODO: what data structure to pick?
std::list<Particle> particles;

void printUsage() {
  std::cerr << "Usage: ./MolSim <input-file> <t_end> <delta_t>\n"
            << "Example: ./MolSim <input-file> eingabe-sonne.txt 1000 0.014 VTK\n"
            << "  <input_file>   : path to initial particle data (text file)\n"
            << "  <t_end>        : simulation end time (e.g. 1000)\n"
            << "  <delta_t>      : time step (e.g. 0.014)\n";
}

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  // parse input arguments
  if (argc == 2) {
    if (std::string(argsv[1]) == "-h" || std::string(argsv[1]) == "--help") {
      printUsage();
      return EXIT_SUCCESS;
    }
  }
  if (argc != 4) {  // to avoid incomplete programm calls
    std::cout << "Erroneous programme call! " << std::endl;
    printUsage();
    return -1;
  }

  try {
    end_time = std::stod(argsv[2]);
    delta_t = std::stod(argsv[3]);
  } catch (const std::exception &e) {
    std::cerr << "Error: please enter valid values" << std::endl;
    printUsage();
    return -1;
  }

  FileReader fileReader;
  fileReader.readFile(particles, argsv[1]);

  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();

    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }

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
// write the output into a separate directory
void plotParticles(int iteration) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");
  // outputWriter::XYZWriter xyz_writer;

// use macros to make vtk output optional and avoid build errors
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter vtk_writer;
  vtk_writer.plotParticles(particles, out_name, iteration);
#else
  outputWriter::XYZWriter xyz_writer;
  xyz_writer.plotParticles(particles, out_name, iteration);
#endif
}
