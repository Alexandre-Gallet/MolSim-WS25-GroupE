/**
 * @file Particle.h
 * @brief Particle class representing a single particle in the molecular dynamics simulation
 */

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class LinkedCellContainer;

/**
 * @brief Class representing a particle in the molecular dynamics simulation
 */
class Particle {
 private:
 public:
  struct Neighbor {
    Particle *particle{nullptr};
    double rest_length{0.0};
  };
 private:

  /**
   * @brief Position of the particle
   */
  std::array<double, 3> x{};

  /**
   * @brief Velocity of the particle
   */
  std::array<double, 3> v{};

  /**
   * @brief Force effective on this particle
   */
  std::array<double, 3> f{};

  /**
   * @brief Force which was effective on this particle
   */
  std::array<double, 3> old_f{};

  /**
   * @brief Mass of this particle
   */
  double m{};

  /**
   * @brief Type of the particle
   */
  int type;
  uint32_t owned_index_ = 0;

  /**
   * @brief Neighboring particles used for membrane simulations
   */
  std::vector<Neighbor> neighbors_;

 public:
  friend class LinkedCellContainer;
  /**
   * @brief Construct a new Particle object with default type
   * @param type Type identifier for the particle
   */
  explicit Particle(int type = 0);

  /**
   * @brief Copy constructor
   * @param other Particle to copy from
   */
  Particle(const Particle &other);

  /**
   * @brief Construct a new Particle object with position, velocity, mass and type
   * @param x_arg Initial position vector (3D coordinates)
   * @param v_arg Initial velocity vector (3D coordinates)
   * @param m_arg Mass of the particle
   * @param type Type identifier for the particle
   */
  Particle(
      // for visualization, we need always 3 coordinates
      // -> in case of 2d, we use only the first and the second
      const std::array<double, 3> &x_arg, const std::array<double, 3> &v_arg, double m_arg, int type = 0);

  /**
   * @brief Destroy the Particle object
   */
  virtual ~Particle();

  /**
   * @brief Get the position of the particle
   * @return Reference to the position array
   */
  const std::array<double, 3> &getX() const { return x; }

  /**
   * @brief Set the position of the particle
   */
  void setX(const std::array<double, 3> &newX) { x = newX; }

  /**
   * @brief Get the velocity of the particle
   * @return Reference to the velocity array
   */
  const std::array<double, 3> &getV() const { return v; }

  /**
   * @brief Set the velocity of the particle
   */
  void setV(const std::array<double, 3> &newV) { v = newV; }

  /**
   * @brief Get the current force acting on the particle
   * @return Reference to the force array
   */
  const std::array<double, 3> &getF() const { return f; }

  /**
   * @brief Set the current force acting on the particle
   */
  void setF(const std::array<double, 3> &newF) { f = newF; }

  /**
   * @brief Add to the current force acting on the particle
   */
  void addF(double fx, double fy, double fz) {
    f[0] += fx;
    f[1] += fy;
    f[2] += fz;
  }

  /**
   * @brief Get the old force that was acting on the particle
   * @return Reference to the old force array
   */
  const std::array<double, 3> &getOldF() const { return old_f; }

  /**
   * @brief Set the old force that was acting on the particle
   */
  void setOldF(const std::array<double, 3> &oldF) { old_f = oldF; }

  /**
   * @brief Get the mass of the particle
   * @return Mass value
   */
  double getM() const { return m; }

  /**
   * @brief Get the type of the particle
   * @return Type identifier
   */
  int getType() const { return type; }

  /**
   * @brief Equality comparison operator
   * @param other Particle to compare with
   * @return true if particles are equal, false otherwise
   */
  bool operator==(const Particle &other) const;

  /**
   * @brief Convert particle data to string representation
   * @return String containing particle information
   */
  std::string toString() const;

  /**
   * @brief Add a neighbor reference with its rest length.
   */
  void addNeighbor(Particle *neighbor, double rest_length);

  /**
   * @brief Remove all stored neighbors.
   */
  void clearNeighbors();

  /**
   * @brief Accessor for neighbor list (mutable).
   */
  std::vector<Neighbor> &getNeighbors();

  /**
   * @brief Accessor for neighbor list (const).
   */
  const std::vector<Neighbor> &getNeighbors() const;
};

/**
 * @brief Output stream operator for Particle
 * @param stream Output stream
 * @param p Particle to output
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &stream, const Particle &p);
