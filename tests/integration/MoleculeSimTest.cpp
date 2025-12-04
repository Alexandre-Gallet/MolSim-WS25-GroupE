// MoleculeSimulationOutflowIntegrationTest.cpp
//
// Integration Test #1:
// Verifies that particles are removed when they leave the simulation
// domain through an outflow boundary.
//
// Scenario:
// - A small cuboid of 2 particles is placed near the left side of the domain.
// - Their velocity points toward the +x direction.
// - The domain has outflow boundary conditions at x-min and x-max.
// - After running the simulation for a short period, at least one particle
//   should have exited the domain and been removed.

#include <Container/ParticleContainer.h>
#include <Simulation/MoleculeSimulation.h>
#include <inputReader/Arguments.h>

#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

TEST(MoleculeSimulationIntegrationTest, OutflowBoundaryRemovesParticlesOverTime) {
  // 1) Create a temporary YAML input file describing the scenario.
  const std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
  const std::filesystem::path yamlFile = tmpDir / "mol_sim_outflow_test.yaml";

  {
    std::ofstream out(yamlFile);
    ASSERT_TRUE(out.good());

    // NOTE: Adapt this block to match your actual YAML format if necessary.
    out <<
        "output:\n"
        "  baseName: \"outflow_integration_test\"\n"
        "  writeFrequency: 1000\n"
        "\n"
        "simulation:\n"
        "  tEnd: 0.5\n"
        "  deltaT: 0.001\n"
        "\n"
        "domain:\n"
        "  size: [20.0, 5.0, 1.0]\n"
        "  rCutoff: 3.0\n"
        "\n"
        "boundaries:\n"
        "  lower: [\"outflow\", \"reflecting\", \"outflow\"]\n"
        "  upper: [\"outflow\", \"reflecting\", \"outflow\"]\n"
        "\n"
        "cuboids:\n"
        "  - origin:   [2.0, 2.5, 0.0]\n"
        "    numPerDim:[2, 1, 1]\n"
        "    h:        1.0\n"
        "    mass:     1.0\n"
        "    velocity: [10.0, 0.0, 0.0]\n"
        "    type:     0\n";
  }

  // 2) Initialize simulation arguments.
  Arguments args;
  std::string path = yamlFile.string();
  args.inputFile = strdup(path.c_str());  // allocate C-string copy

  // 3) Create particle container and run the simulation.
  ParticleContainer particles;
  const std::size_t initialCount = 2;  // Known from the YAML file.

  MoleculeSimulation simulation(args, particles);
  simulation.runSimulation();

  free(args.inputFile);  // not leaking memory! didn't want to change the Arguments struct it is kinda C-like but temp solution


  // 4) At least one particle should have exited the domain and been deleted.
  ASSERT_LE(particles.size(), initialCount);
  EXPECT_LT(particles.size(), initialCount)
      << "Outflow boundary did not remove any particles as expected.";
}