#include <gtest/gtest.h>
#include <cmath>
#include <array>

#include "ForceCalculation/LennardJones.h"
#include "ParticleContainer.h"
#include "Particle.h"

// --- Helper: vector norm ---
double norm3D(const std::array<double,3>& v) {
    return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// --- Helper: dot product ---
double dot3D(const std::array<double,3>& a, const std::array<double,3>& b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

/*  TEST 1: Newton's 3rd Law (force symmetry)  F12 = -F21 MUST always hold for Lennard-Jones interactions. */
TEST(LennardJonesBehaviourTest, Newton3rdLawSymmetry) {
    LennardJones lj;
    ParticleContainer container;

    // parameters are stored inside lj object; override via constructor or setters
    // your class currently uses default epsilon/sigma = 0,
    // so we manually access `calc()` instead of `calculateF()`.

    double epsilon = 5.0;
    double sigma   = 1.0;

    Particle p1, p2;
    p1.setX({0,0,0});
    p2.setX({1.5,0,0});
    p1.setF({0,0,0});
    p2.setF({0,0,0});

    // Pairwise force application
    LennardJones::calc(p1, p2, epsilon, sigma);

    auto F12 = p1.getF();
    auto F21 = p2.getF();

    EXPECT_NEAR(F12[0] + F21[0], 0.0, 1e-10);
    EXPECT_NEAR(F12[1] + F21[1], 0.0, 1e-10);
    EXPECT_NEAR(F12[2] + F21[2], 0.0, 1e-10);
}


/*  TEST 2: Zero force at equilibrium distance  r_min = 2^(1/6) * sigma */
 /*TEST(LennardJonesBehaviourTest, ZeroForceAtEquilibriumDistance) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;

    Particle p1, p2;
    p1.setX({0,0,0});
    p2.setX({r_min,0,0});
    p1.setF({0,0,0});
    p2.setF({0,0,0});

    LennardJones::calc(p1, p2, epsilon, sigma);

    EXPECT_NEAR(norm3D(p1.getF()), 0.0, 1e-6);
    EXPECT_NEAR(norm3D(p2.getF()), 0.0, 1e-6);
}
*/


/*  dot(F12, r12) < 0 means pushing apart. */
TEST(LennardJonesBehaviourTest, RepulsiveForShortDistances) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;
    double r     = 0.7 * r_min;  // too close → repulsive

    Particle p1, p2;
    p1.setX({0,0,0});
    p2.setX({r,0,0});
    p1.setF({0,0,0});
    p2.setF({0,0,0});

    LennardJones::calc(p1, p2, epsilon, sigma);

    std::array<double,3> r12 = {
        p2.getX()[0] - p1.getX()[0],
        p2.getX()[1] - p1.getX()[1],
        p2.getX()[2] - p1.getX()[2]
    };

    auto F12 = p1.getF();

    EXPECT_LT(dot3D(F12, r12), 0.0);
}



 /* TEST 4: Attractive force for long distances (r > r_min) dot(F12, r12) > 0 means pulling together. */
TEST(LennardJonesBehaviourTest, AttractiveForLongDistances) {
    double epsilon = 5.0;
    double sigma   = 1.0;

    double r_min = std::pow(2.0, 1.0/6.0) * sigma;
    double r     = 2.0 * r_min;  // far → attraction

    Particle p1, p2;
    p1.setX({0,0,0});
    p2.setX({r,0,0});
    p1.setF({0,0,0});
    p2.setF({0,0,0});

    LennardJones::calc(p1, p2, epsilon, sigma);

    std::array<double,3> r12 = {
        p2.getX()[0] - p1.getX()[0],
        p2.getX()[1] - p1.getX()[1],
        p2.getX()[2] - p1.getX()[2]
    };

    auto F12 = p1.getF();

    EXPECT_GT(dot3D(F12, r12), 0.0);
}
