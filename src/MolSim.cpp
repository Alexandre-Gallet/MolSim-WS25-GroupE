
// #include <bits/valarray_after.h>
#include <valarray>
#include <iostream>
#include <list>
#include <filesystem>
#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
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

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 4) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  end_time = std::stod(argsv[2]);
  delta_t = std::stod(argsv[3]);

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
  // std::list<Particle>::iterator iterator;
  // iterator = particles.begin();

  for (auto &p : particles) {
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  for (auto &p1 : particles) {

    for (auto &p2 : particles) {
      // @TODO: insert calculation of forces here!
      if (!(p1 == p2)) {
        double norm = std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 3);
        double scalar = p1.getM() * p2.getM() / norm;
        std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
          scalar,
          ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()),
          std::multiplies<>());
        p1.setF(ArrayUtils::elementWisePairOp(p1.getF(),newF,  std::plus<>()));
        //p2.setF(ArrayUtils::elementWisePairOp(p2.getF(),newF,  std::minus<>()));
      }
    }
  }
}

void calculateX() {
  for (auto &p : particles) {
    // @TODO: insert calculation of position updates here!

    p.setX(ArrayUtils::elementWisePairOp(p.getX(), ArrayUtils::elementWisePairOp(ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
      ArrayUtils::elementWiseScalarOp(pow(delta_t, 2) / (2 * p.getM()), p.getF(), std::multiplies<>()), std::plus<>()), std::plus<>() ));
  }
}

void calculateV() {
  for (auto &p : particles) {
    // @TODO: insert calculation of veclocity updates here!
    p.setV(ArrayUtils::elementWisePairOp(p.getV(), ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()), ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()), std::multiplies<>()), std::plus<>()));
  }
}

void plotParticles(int iteration) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");
  //outputWriter::XYZWriter xyz_writer;


#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter vtk_writer;
  vtk_writer.plotParticles(particles, out_name, iteration);
#endif
}
