
#include "PlanetSimulation.h"

#include <filesystem>
#include <iostream>

#include "../ForceCalculation/StormerVerlet.h"
#include "../inputReader/FileReader.h"
#include "../outputWriter/WriterFactory.h"

PlanetSimulation::PlanetSimulation(const Arguments &args, ParticleContainer &particles)
    : inputFile(args.inputFile),
      t_start(args.t_start),
      t_end(args.t_end),
      delta_t(args.delta_t),
      outputFormat(args.output_format),
      particles(particles) {}

void PlanetSimulation::runSimulation() {
  FileReader fileReader;
  fileReader.readFile(getParticles(), getInputFile());
  double current_time = getTStart();
  int iteration = 0;
  // for this loop, we assume: current x, current f and current v are known
  while (current_time < getTEnd()) {
    StormerVerlet verlet;
    // calculate new X
    StormerVerlet::calculateX(getParticles(), getDeltaT());
    // calculate new F
    verlet.calculateF(getParticles());
    // calculate new V
    StormerVerlet::calculateV(getParticles(), getDeltaT());
    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(getParticles(), iteration, getOutputFormat());
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += getDeltaT();
  }
}
void PlanetSimulation::plotParticles(ParticleContainer &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");
  std::string out_name("output/MD_vtk");

  const auto writer = WriterFactory::createWriter(format);

  writer->plotParticles(particles, out_name, iteration);
}
