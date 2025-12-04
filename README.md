# Molecular Dynamics Simulator – Group E

Implementation of a molecular dynamics (MD) simulator developed during the lab course  
**Scientific Computing (PSE) – Molekulardynamik (IN0012 / IN4229)**  
at **Technische Universität München**, Winter Semester **2025/2026**.

## Table of Contents
- [Building the Project](#building-the-project)
- [Running the Simulation](#running-the-simulation) 
- [YAML Configuration Format)(#yaml-configuration-format) 
- [Running Tests](#running-tests)
- [Doxygen Documentation](#doxygen-documentation)
- [Clang-Tidy and Clang-Format](#clang-tidy-and-clang-format)

## Building the Project

### 1. Clone the repository
```bash
git clone https://github.com/Alexandre-Gallet/MolSim-WS25-GroupE.git
cd MolSim-WS25-GroupE
```

### 2. Install dependencies

**Ubuntu/Debian**
```bash
sudo apt install libvtk9-dev cmake build-essential
```

**Arch Linux**
```bash
sudo pacman -S base-devel cmake vtk
```

**Fedora**
```bash
sudo dnf install vtk-devel cmake gcc-c++ make
```

### 3. Configure and build the project

You can optionally specify:

- `-DCMAKE_BUILD_TYPE`  
  - Default: `Release` (`-O3`, `-DNDEBUG`)  
  - Other valid values: `Debug`, `RelWithDebInfo`, `MinSizeRel`

- `-DLOG_LEVEL`  
  - Default: `INFO`  
  - Other options: `TRACE`, `DEBUG`, `WARN`, `ERROR`, `CRITICAL`, `OFF`  
  - Defines which logging statements are compiled into the binary.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLOG_LEVEL=INFO
cmake --build build -- -j"$(nproc)"
cmake --build build --target doc_doxygen
```

## Running the Simulation

After building, run the simulation from within the `build` directory:

```bash
./MolSim path/to/config.yml 
```

### Example

Using the provided example file eingabe.yml

```bash
./MolSim ../input/eingabe.yml 
```

Simulation output files (VTK, XYZ, or both depending on `output_format` as defined in the .yml file)
will appear in the working directory.

If logging is enabled (default), a `simulation.log` file is also generated
in the working directory.


## YAML Configuration Format

MolSim is configured entirely through a YAML file. The structure is divided into five main sections: **simulation**, **output**, **cuboids**, **discs**, **linkedCell**

### Top-Level Structure

```yaml
simulation:
  sim_type: molecule | planet
  t_start: <double>
  t_end: <double>
  delta_t: <double>
  output_format: VTK | XYZ

output:
  write_frequency: <int>

cuboids:
  - origin: [x, y, z]
    numPerDim: [nx, ny, nz]
    baseVelocity: [vx, vy, vz]
    h: <double>
    mass: <double>
    type: <int>
    brownianMean: <double>

discs:
  - center: [x, y, z]
    radiusCells: <int>
    hDisc: <double>
    mass: <double>
    baseVelocityDisc: [vx, vy, vz]
    typeDisc: <int>

linkedCell:
  - containerType: [Cell]
    domainSize: [Lx, Ly, Lz]
    rCutoff: <double>
    boundaryConditions: [bc_x1, bc_x2, bc_y1, bc_y2, bc_z1, bc_z2]yy
```

YAML Section Overview (Table Format)
-----------------------------------

| Section     | Field               | Meaning                                                                 |
|-------------|---------------------|-------------------------------------------------------------------------|
| simulation  | sim_type            | Selects the simulation model (“molecule” or “planet”).                 |
|             | t_start             | Start time of the simulation.                                          |
|             | t_end               | End time of the simulation.                                            |
|             | delta_t             | Time step size.                                                        |
|             | output_format       | Format used for particle output files.                                 |
|-------------|---------------------|------------------------------------------------------------------------|
| output      | write_frequency     | Writes output every n-th iteration.                                    |
|-------------|---------------------|-------------------------------------------------------------------------|
| cuboids     | origin              | Position of the cuboid’s lower-left-front corner.                      |
|             | numPerDim           | Number of particles along each dimension.                              |
|             | baseVelocity        | Initial particle velocity.                                             |
|             | h                   | Particle spacing (mesh width).                                         |
|             | mass                | Mass of each particle.                                                 |
|             | type                | Integer particle type identifier.                                      |
|             | brownianMean        | Mean of Brownian/random velocity distribution.                         |
|-------------|---------------------|-------------------------------------------------------------------------|
| discs       | center              | Center position of the disc.                                           |
|             | radiusCells         | Disc radius measured in grid cells.                                    |
|             | hDisc               | Mesh width for disc particles.                                         |
|             | mass                | Mass of each particle in the disc.                                     |
|             | baseVelocityDisc    | Initial velocity of disc particles.                                    |
|             | typeDisc            | Particle type identifier for disc particles.                           |
|-------------|---------------------|-------------------------------------------------------------------------|
| linkedCell  | containerType       | Container implementation (currently “Cell”).                           |
|             | domainSize          | Size of the simulation domain.                                         |
|             | rCutoff             | Lennard–Jones cutoff radius.                                           |
|             | boundaryConditions  | Boundary types for ±x, ±y, ±z directions.                              |


An example of a working yaml configuration file can be found at `input/eingabe.yml`

## Running Tests

```bash
ctest --test-dir build --output-on-failure -j"$(nproc)"
```

## Doxygen Documentation

After having built the project, generate the documentation via:

```bash
cmake --build build --target doc_doxygen
```

open it from the `build` directory with:

```bash
xdg-open docs/html/index.html
```

## Clang-Tidy and Clang-Format

To run static analysis and formatting checks manually use a clean build of the project:

```bash
rm -rf build/
mkdir -p build
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target run_clang_tidy
cmake --build build --target clang_format_check
```

These commands respect the configuration files `.clang-tidy` and `.clang-format`.
