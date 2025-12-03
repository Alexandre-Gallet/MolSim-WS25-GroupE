#include "MoleculeSimulation.h"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/LennardJones.h"
#include "ParticleGenerator.h"
#include "inputReader/FileReaderCuboids.h"
#include "outputWriter/WriterFactory.h"

MoleculeSimulation::MoleculeSimulation(Arguments &args, Container &particles) : args(args), particles(particles) {}

void MoleculeSimulation::runSimulation() {
  std::vector<Cuboid> cuboids;

  // Read simulation setup from input file
  FileReaderCuboid::readFile(cuboids, args.inputFile);
  SPDLOG_INFO("Loaded {} cuboid(s) from input file '{}'.", cuboids.size(), args.inputFile);

  // Generate particles for each cuboid
  for (const auto &c : cuboids) {
    ParticleGenerator::generateCuboid(particles, c.origin, c.numPerDim, args.domain_size, c.h, c.mass, c.baseVelocity, c.brownianMean,
                                      c.type);
  }
  SPDLOG_INFO("Generated {} particles from cuboids.", particles.size());

  LennardJones lj;
  lj.setEpsilon(5);
  lj.setSigma(1);
  lj.calculateF(particles);  // initial force evaluation for the first integration step
  SPDLOG_DEBUG("Initial Lennard-Jones forces computed (epsilon=5, sigma=1).");

  double current_time = args.t_start;
  int iteration = 0;

  SPDLOG_INFO("Starting molecule simulation: t_start={}, t_end={}, delta_t={}.", args.t_start, args.t_end,
              args.delta_t);

  while (current_time < args.t_end) {
    // calculate forces, position and velocity
    LennardJones::calculateX(particles, args.delta_t);
    if (args.cont_type == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles)->rebuild();
    }
    lj.calculateF(particles);
    LennardJones::calculateV(particles, args.delta_t);

    iteration++;
    if (iteration % 10 == 0) {
      SPDLOG_INFO("Writing output at iteration {} (t = {}).", iteration, current_time);
      plotParticles(particles, iteration, args.output_format);
    }

    SPDLOG_DEBUG("Iteration {} finished (t = {}).", iteration, current_time);

    current_time += args.delta_t;
  }

  SPDLOG_INFO("Molecule simulation completed after {} iterations (final t = {:.6g}).", iteration, current_time);
}

void MoleculeSimulation::plotParticles(Container &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}