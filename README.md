# Molecular Dynamics Simulator – Group E

Implementation of a molecular dynamics (MD) simulator developed during the lab course  
**Scientific Computing (PSE) – Molekulardynamik (IN0012 / IN4229)**  
at **Technische Universität München**, Winter Semester **2025/2026**.

## Table of Contents
- [Building the Project](#building-the-project)
- [Running the Simulation](#running-the-simulation) 
- [Benchmarking and Profiling](#benchmarking-and-profiling)
- [YAML Configuration Format](#yaml-configuration-format) 
- [Checkpointing](#checkpointing)
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
```

## Running the Simulation

After building, run the simulation from within the `build` directory:

```bash
./MolSim path/to/config.yml 
```

### Example

Using some of the provided example files:

```bash
./MolSim ../input/eingabe.yml 
./MolSim ../input/eingabeRayleighTaylorSmall.yml
./MolSim ../input/eingabeNanoScaleFlow.yml
```

Simulation output files (VTK, XYZ, or both depending on `output_format` as defined in the .yml file)
will appear in the working directory.

If logging is enabled (default), a `simulation.log` file is also generated
in the working directory.

## Benchmarking and Profiling 

### Benchmarking 

For performance benchmarking a specific set of flags has to be passed during configuration.

```bash
cmake -S . -B build-benchmark \
  -DCMAKE_BUILD_TYPE=Release \
  -DLOG_LEVEL=OFF \
  -DENABLE_VTK_OUTPUT=OFF

cmake --build build-benchmark -- -j"$(nproc)"
```

Running the simulation produces a measurement of elapsed time and MUPS/s (Molecule Updates per Second). 
The above cofiguration turns of all IO, Logging and VTK Output. For Benchmarking on CoolMUC there are a series of 
SLURM batch scripts located at ´runs/task4´. 


### Profiling

For profiling a specific set of flags has to be passed during configuration.

```bash
cmake -S . -B build-benchmark \
  -DCMAKE_BUILD_TYPE=Release \
  -DLOG_LEVEL=OFF \
  -DENABLE_VTK_OUTPUT=OFF
  -DCMAKE_CXX_FLAGS="-pg"

cmake --build build-gprof -- -j"$(nproc)"
```
The above cofiguration turns of all IO, Logging and VTK Output. 
The instrumented binary generates a gmon.out file after execution.
The profiling report is generated with:

```bash
gprof build-gprof/MolSim gmon.out > gprof_output.txt
```

For profiling on CoolMUC there are a series of SLURM batch scripts located at ´runs/task4´.

## YAML Configuration Format

MolSim is configured entirely through a YAML file. The structure is divided into five main sections: **simulation**, **output**, **cuboids**, **discs**, **linkedCell**

| Section       | Field              | Meaning                                                                              |
|---------------|--------------------|--------------------------------------------------------------------------------------|
| simulation    | sim_type           | Selects the simulation model (“molecule” or “planet”).                               |
|               | t_start            | Start time of the simulation.                                                        |
|               | t_end              | End time of the simulation.                                                          |
|               | delta_t            | Time step size.                                                                      |
|               | output_format      | Format used for particle output files.                                               |
|               | gravity            | Gravity factor.                                                                      |
|               | epsilon            | LJ epsilon parameter for the simulation.                                             |
|               | sigma              | LJ sigma parameter for the simulation.                                               |
|               |                    |                                                                                      |
| output        | write_frequency    | Writes output every n-th iteration.                                                  |
|               |                    |                                                                                      |
| cuboids       | origin             | Position of the cuboid’s lower-left-front corner.                                    |
|               | numPerDim          | Number of particles along each dimension.                                            |
|               | baseVelocity       | Initial particle velocity.                                                           |
|               | h                  | Particle spacing (mesh width).                                                       |
|               | mass               | Mass of each particle.                                                               |
|               | type               | Integer particle type identifier.                                                    |
|               | brownianMean       | Mean of Brownian/random velocity distribution.                                       |
|               |                    |                                                                                      |
| discs         | center             | Center position of the disc.                                                         |
|               | radiusCells        | Disc radius measured in grid cells.                                                  |
|               | hDisc              | Mesh width for disc particles.                                                       |
|               | mass               | Mass of each particle in the disc.                                                   |
|               | baseVelocityDisc   | Initial velocity of disc particles.                                                  |
|               | typeDisc           | Particle type identifier for disc particles.                                         |
|               |                    |                                                                                      |
| types         | id                 | Particle id.                                                                         |
|               | epsilon            | Epsilon parameter for the L-J force calculation for the particles with the given id. |
|               | sigma              | Sigma parameter for the L-J force calculation for the particles with the given id.   |
|               |                    |                                                                                      |
| linkedCell    | containerType      | Container implementation (currently “Cell”).                                         |
|               | domainSize         | Size of the simulation domain.                                                       |
|               | rCutoff            | Lennard–Jones cutoff radius.                                                         |
|               | boundaryConditions | Boundary types for ±x, ±y, ±z directions.                                            |
|               |                    |                                                                                      |
| thermostat    | t_init             | Initial temperature of the system.                                                   |
|               | dimensions         | The number of dimensions in the simulation.                                          |
|               | n_thermostat       | Interval (in time steps) at which the thermostat is applied.                         |
|               | t_target           | The desired temperature.                                                             |
|               | delta_t            | Maximum temperature change per thermostat application.                               |
|               | brownian_motion    | If true, initializes velocities using Maxwell-Boltzmann distribution.                |
| ns_thermostat | t_init             | Initial temperature of the system.                                                   |
|               | dimensions         | The number of dimensions in the simulation.                                          |
|               | n_thermostat       | Interval (in time steps) at which the thermostat is applied.                         |
|               | t_target           | The desired temperature.                                                             |
|               | delta_t            | Maximum temperature change per thermostat application.                               |
|               | brownian_motion    | If true, initializes velocities using Maxwell-Boltzmann distribution.                |

Examples of a working yaml configuration files can be found at `input/eingabe.yml` and `input/eingabedisc.yml` 

##  Checkpointing
Checkpointing is used in Worksheet 4 to split the falling drop simulation into an equilibration phase and a subsequent production run that restarts from a saved simulation state.

- The checkpoint filename depends on the total number of steps and `output.write_frequency`. Total steps = `(t_end - t_start) / delta_t`. Example: `t_end=15`, `delta_t=0.0005` ⇒ 30,000 steps. With `write_frequency=1000` the final file is `checkpoint_30000.state`; with `write_frequency=500` it would be `checkpoint_60000.state`. Checkpoints are written under `build/output/`.
- The drop run resumes from the checkpoint created by the equilibration run. Set `simulation.checkpoint_file` to that `.state` file and `restart_from_checkpoint: true`. Only the initial phase space is restored; timing/output parameters (`t_start`, `t_end`, `delta_t`, `write_frequency`) come from the new YAML.
- Run from the `build` directory, passing the YAML as input (you do not “run” the YAML itself):
  - Equilibration: `./MolSim ../input/eingabeW4Task3_equilibrate.yml`
  - Drop: `./MolSim ../input/eingabeW4Task3_drop.yml`
```bash
./MolSim ../input/eingabeW4Task3_equilibrate.yml
./MolSim ../input/eingabeW4Task3_drop.yml
```

## Running Tests

After having built the project, run the tests from the top-level directory via: 

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
