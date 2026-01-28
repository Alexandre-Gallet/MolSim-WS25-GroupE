# Molecular Dynamics Simulator – Group E

Implementation of a molecular dynamics (MD) simulator developed during the lab course  
**Scientific Computing (PSE) – Molekulardynamik (IN0012 / IN4229)**  
at **Technische Universität München**, Winter Semester **2025/2026**.

## Table of Contents
- [Configuration and Building](#configuration-and-building)
- [Running the Simulation](#running-the-simulation) 
- [Running Tests](#running-tests)
- [Doxygen Documentation](#doxygen-documentation)
- [Clang-Tidy and Clang-Format](#clang-tidy-and-clang-format)
- [Benchmarking and Profiling](#benchmarking-and-profiling)
- [YAML Configuration Format](#yaml-configuration-format) 
- [Checkpointing](#checkpointing)


## Configuration and Building

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

```bash
cmake -S . -B build 
cmake --build build -- -j"$(nproc)"
```

In the configuration phase you can optionally specify

- `-DCMAKE_BUILD_TYPE`
  - Default: `Release` (using `-O3`, `-DNDEBUG` flags)
  - Other valid values: `Debug`, `RelWithDebInfo`, `MinSizeRel`

- `-DLOG_LEVEL`
  - Default: `INFO`
  - Other options: `TRACE`, `DEBUG`, `WARN`, `ERROR`, `CRITICAL`, `OFF`

## Running the Simulation

After configuring and building, run the simulation from the top-level directory with: 

```bash
./build/src/MolSim path/to/config.yml
```

### Example

Using some of the provided input files:

```bash
./build/src/MolSim ./input/eingabe.yml
./build/src/MolSim ./input/eingabeRayleighTaylorSmall.yml
```

Output location: MolSim creates an output/ directory in the current working directory (the directory you run the executable from).
If logging is enabled, a simulation.log is also written there.

## Running Tests

Run the tests from the top-level directory with:

```bash
./build/tests/MolSimTests
```


## Clang-Tidy and Clang-Format

Format checking and fixing is run from the top-level directory with custom targets:

```bash
cmake --build build --target clang_format_check
cmake --build build --target clang_format_fix
cmake --build build --target run_clang_tidy
```

These commands respect the configuration files `.clang-tidy` and `.clang-format`.
clang-tidy is executed per translation unit to avoid excessive noise from system and
STL headers, improving the signal-to-noise ratio of diagnostics.

## Doxygen Documentation

Generate Doxygen from the top-level directory with: 

```bash
cmake --build build --target doc_doxygen
```

Open them with: 

```bash
xdg-open build/docs/html/index.html
```


## Benchmarking and Profiling 

This project provides dedicated executables so that benchmarking/profiling does not depend on ad-hoc 
compiler flags or runtime switches. Build normally with Release (the default value). 
The benchmarking and profiling executables are compiled with `SPDLOG_ACTIVE_LEVEL=OFF`, 
independent of the global `LOG_LEVEL`. We always recommend clean building: 
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j"$(nproc)"
```

**Important:** Set `simulation.output_format: NONE` in the YAML configuration file.

For benchmarking on CoolMUC, SLURM batch scripts are located at runs/task4.

### Benchmarking 

Benchmarking is run from the top-level directory with the dedicated `MolSimBenchmark`:

```bash
./build/src/MolSimBenchmark ./path/to/config.yml
```

### Profiling

Profiling is run from the top-level directory with the dedicated `MolSimGprof`:
```bash
./build/src/MolSimGprof path/to/config.yml
```

This produces gmon.out in the top-level directory. Generate a report via:
```bash
gprof ./build/src/MolSimGprof gmon.out > gprof_output.txt
```

For profiling on CoolMUC, SLURM batch scripts are located at runs/task4.
## YAML Configuration Format

MolSim is configured entirely through a YAML file.
The configuration is organized into the following sections:

| Section    | Field              | Meaning                                                                      |
|------------|--------------------|------------------------------------------------------------------------------|
| simulation | sim_type           | Selects the simulation model (“molecule” or “planet”).                       |
|            | t_start            | Start time of the simulation.                                                |
|            | t_end              | End time of the simulation.                                                  |
|            | delta_t            | Time step size.                                                              |
|            | output_format      | Output format: `XYZ`, `VTK`, `CHECKPOINT`, or `NONE`                                             |
|            | gravity            | Gravity factor.                                                              |
|            | epsilon            | LJ epsilon parameter for the simulation.                                     |
|            | sigma              | LJ sigma parameter for the simulation.                                       |
|            |                    |                                                                              |
| output     | write_frequency    | Writes output every n-th iteration.                                          |
|            |                    |                                                                              |
| cuboids    | origin             | Position of the cuboid’s lower-left-front corner.                            |
|            | numPerDim          | Number of particles along each dimension.                                    |
|            | baseVelocity       | Initial particle velocity.                                                   |
|            | h                  | Particle spacing (mesh width).                                               |
|            | mass               | Mass of each particle.                                                       |
|            | type               | Integer particle type identifier.                                            |
|            | brownianMean       | Mean of Brownian/random velocity distribution.                               |
|            |                    |                                                                              |
| discs      | center             | Center position of the disc.                                                 |
|            | radiusCells        | Disc radius measured in grid cells.                                          |
|            | hDisc              | Mesh width for disc particles.                                               |
|            | mass               | Mass of each particle in the disc.                                           |
|            | baseVelocityDisc   | Initial velocity of disc particles.                                          |
|            | typeDisc           | Particle type identifier for disc particles.                                 |
|            |                    |                                                                              |
| types      | id                 | Particle id.                                                                 |
|            | epsilon            | Epsilon parameter for the L-J force calculation for the particles with the given id. |
|            | sigma              | Sigma parameter for the L-J force calculation for the particles with the given id. |
|            |                    |                                                                              |
| linkedCell | containerType      | Container implementation (currently “Cell”).                                 |
|            | domainSize         | Size of the simulation domain.                                               |
|            | rCutoff            | Lennard–Jones cutoff radius.                                                 |
|            | boundaryConditions | Boundary types for ±x, ±y, ±z directions.                                    |
|            |                    |                                                                              |
| thermostat | t_init             | Initial temperature of the system.                                           |
|            | dimensions         | The number of dimensions in the simulation.                                  |
|            | n_thermostat       | Interval (in time steps) at which the thermostat is applied.                 |
|            | t_target           | The desired temperature.                                                     |
|            | delta_t            | Maximum temperature change per thermostat application.                       |
|            | brownian_motion    | If true, initializes velocities using Maxwell-Boltzmann distribution.        |


Examples of a working YAML configuration files can be found at `input/`

##  Checkpointing
Checkpointing is used to split the falling drop simulation into an equilibration phase and a subsequent production run that restarts from a saved simulation state.

The checkpoint filename depends on the total number of steps and `output.write_frequency`. 
Total steps = `(t_end - t_start) / delta_t`. Example: `t_end=15`, `delta_t=0.0005` ⇒ 30,000 steps. 
With `write_frequency=1000` the final file is `checkpoint_30000.state`; 
with `write_frequency=500` it would be `checkpoint_60000.state`. 
Checkpoints are written in `./output/`.
<br />

The drop run resumes from the checkpoint created by the equilibration run. 
Set `simulation.checkpoint_file` to that `.state` file and `restart_from_checkpoint: true`. 
Only the initial phase space is restored; 
timing/output parameters (`t_start`, `t_end`, `delta_t`, `write_frequency`) 
come from the new YAML. <br />

Use the checkpointing mechanics by running from the top-level directory. 
An example of this would work using the provided input files is given by: 
```bash
./build/src/MolSim ../input/eingabeW4Task3_equilibrate.yml
./build/src/MolSim ../input/eingabeW4Task3_drop.yml
```
