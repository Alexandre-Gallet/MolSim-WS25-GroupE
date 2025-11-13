

#include "MoleculeSimulation.h"

#include <filesystem>
#include <iostream>

#include "ForceCalculation/LennardJones.h"
#include "ParticleGenerator.h"
#include "outputWriter/WriterFactory.h"

MoleculeSimulation::MoleculeSimulation(Arguments &args, ParticleContainer &particles)
    : args(args), particles(particles) {}
void MoleculeSimulation::runSimulation() {
  for (const auto &c : args.cuboids) {
    ParticleGenerator::generateCuboid(particles, c.origin, c.numPerDim, c.h, c.mass, c.baseVelocity, c.brownianMean,
                                      c.type);
  }
  double current_time = args.t_start;
  int iteration = 0;
  while (current_time < args.t_end) {
    LennardJones lj;
    LennardJones::calculateX(particles, args.delta_t);
    lj.calculateF(particles);
    LennardJones::calculateV(particles, args.delta_t);
    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(particles, iteration, args.output_format);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += args.delta_t;
  }
}
void MoleculeSimulation::plotParticles(ParticleContainer &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");

  const auto writer = WriterFactory::createWriter(format);

  writer->plotParticles(particles, out_name, iteration);
}