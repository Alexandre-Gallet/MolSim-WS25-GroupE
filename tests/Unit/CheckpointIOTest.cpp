#include <gtest/gtest.h>

#include <filesystem>

#include "Container/ParticleContainer.h"
#include "inputReader/CheckpointReader.h"
#include "outputWriter/CheckpointWriter.h"

namespace {
void expectArrayEq(const std::array<double, 3> &a, const std::array<double, 3> &b) {
  EXPECT_DOUBLE_EQ(a[0], b[0]);
  EXPECT_DOUBLE_EQ(a[1], b[1]);
  EXPECT_DOUBLE_EQ(a[2], b[2]);
}
}  // namespace

TEST(CheckpointIOTest, RoundtripPersistsParticleState) {
  ParticleContainer original;

  Particle p1({1.0, 2.0, 0.0}, {0.1, 0.2, 0.0}, 2.0, 0);
  p1.setF({0.5, -0.1, 0.0});
  p1.setOldF({0.4, -0.2, 0.0});
  original.addParticle(p1);

  Particle p2({-1.0, 0.0, 0.0}, {-0.1, 0.0, 0.0}, 1.5, 1);
  p2.setF({0.0, 1.0, 0.0});
  p2.setOldF({0.0, 0.5, 0.0});
  original.addParticle(p2);

  const auto tmpDir = std::filesystem::temp_directory_path() / "MolSimCheckpointTest";
  std::filesystem::create_directories(tmpDir);
  const std::string base = (tmpDir / "checkpoint_roundtrip").string();

  outputWriter::CheckpointWriter writer;
  writer.plotParticles(original, base, 0);

  const std::string checkpointFile = (tmpDir / "checkpoint_roundtrip_0.state").string();
  ParticleContainer restored;
  inputReader::CheckpointReader::readFile(restored, checkpointFile);

  ASSERT_EQ(original.size(), restored.size());
  auto itOrig = original.begin();
  auto itRestored = restored.begin();
  for (; itOrig != original.end(); ++itOrig, ++itRestored) {
    expectArrayEq(itOrig->getX(), itRestored->getX());
    expectArrayEq(itOrig->getV(), itRestored->getV());
    expectArrayEq(itOrig->getF(), itRestored->getF());
    expectArrayEq(itOrig->getOldF(), itRestored->getOldF());
    EXPECT_DOUBLE_EQ(itOrig->getM(), itRestored->getM());
    EXPECT_EQ(itOrig->getType(), itRestored->getType());
  }
}
