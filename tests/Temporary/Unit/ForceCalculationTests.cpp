#include <gtest/gtest.h>
#include <array>
#include <cmath>

#include "LennardJones.h"


// Helper: compute the magnitude of a 3D vector
double norm(const std::array<double,3>& v) {
    return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// Helper: compute dot product of two 3D vectors
double dot(const std::array<double,3>& a, const std::array<double,3>& b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


// Test 1: Force symmetry (Newton's 3rd law)
// For Lennard-Jones, the force from particle j on i must be the exact negative
// of the force from i on j. This ensures momentum conservation.
TEST(LennardJonesForceTest, SymmetryFijEqualsMinusFji) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    std::array<double,3> xi{0.0, 0.0, 0.0};
    std::array<double,3> xj{1.5, 0.0, 0.0};

    // Compute forces in both directions
    auto Fij = lennardJonesForce(xi, xj, epsilon, sigma);
    auto Fji = lennardJonesForce(xj, xi, epsilon, sigma);

    // They must cancel out perfectly component-wise
    EXPECT_NEAR(Fij[0] + Fji[0], 0.0, 1e-10);
    EXPECT_NEAR(Fij[1] + Fji[1], 0.0, 1e-10);
    EXPECT_NEAR(Fij[2] + Fji[2], 0.0, 1e-10);
}


// Test 2: Zero force at potential minimum
// The Lennard–Jones potential reaches its minimum at
//      r_min = 2^(1/6) * sigma
// At this distance the force MUST be zero (stable equilibrium).
TEST(LennardJonesForceTest, ZeroForceAtPotentialMinimum) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;

    std::array<double,3> xi{0.0, 0.0, 0.0};
    std::array<double,3> xj{r_min, 0.0, 0.0};

    auto Fij = lennardJonesForce(xi, xj, epsilon, sigma);

    // The norm of the force should be nearly zero
    EXPECT_NEAR(norm(Fij), 0.0, 1e-6);
}


// Test 3: Repulsive force when particles are too close
// For r < r_min, the LJ force must push particles apart.
// This means: dot(Fij, r_ij) < 0
TEST(LennardJonesForceTest, RepulsiveForShortDistances) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;
    double r     = 0.8 * r_min;   // closer than minimum -> repulsion

    std::array<double,3> xi{0.0, 0.0, 0.0};
    std::array<double,3> xj{r,   0.0, 0.0};

    auto Fij = lennardJonesForce(xi, xj, epsilon, sigma);

    std::array<double,3> rij{xj[0] - xi[0], xj[1] - xi[1], xj[2] - xi[2]};

    // Repulsion means the force is in the opposite direction of rij
    EXPECT_LT(dot(Fij, rij), 0.0);
}


// Test 4: Attractive force at long distances
// For r > r_min, the LJ force must pull particles together.
// That means: dot(Fij, r_ij) > 0
TEST(LennardJonesForceTest, AttractiveForLongDistances) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;
    double r     = 2.0 * r_min;  // larger than minimum -> attraction

    std::array<double,3> xi{0.0, 0.0, 0.0};
    std::array<double,3> xj{r,   0.0, 0.0};

    auto Fij = lennardJonesForce(xi, xj, epsilon, sigma);

    std::array<double,3> rij{xj[0] - xi[0], xj[1] - xi[1], xj[2] - xi[2]};

    // Attraction means the force is in the same direction as rij
    EXPECT_GT(dot(Fij, rij), 0.0);
}
